#include "node_init.hpp"
#include "logger.hpp"
#include <memory>
#include <node/node.h>
#include <node/uv.h>
#include <string>
#include <v8.h>
#include <vector>

#define ISOLATE_CREATE_FAILED 1

namespace {
std::unique_ptr<node::MultiIsolatePlatform> node_platform;
} // namespace

static int RunNodeInstance(node::MultiIsolatePlatform *platform,
                           const std::vector<std::string> &args,
                           const std::vector<std::string> &exec_args);

class Args {
public:
  Args(const std::vector<std::string> &args) {
    size_t curr_str_size;
    data_ = nullptr;
    size_ = args.size();

    for (int i = 0; i < size_; i++) {
      curr_str_size = args[i].size() + 1;
      data_ = (char **)realloc(data_, sizeof(char *) * (i + 1));
      data_[i] = (char *)malloc(curr_str_size);
      memcpy(data_[i], args[i].c_str(), curr_str_size);
    }
  }

  char **data() const { return data_; }

  const size_t size() const { return size_; }

  const char *operator[](const size_t idx) {
    if (idx >= size_) {
      throw std::out_of_range("access out of range!\n");
    }
    return data_[idx];
  }

  ~Args() {
    for (int i = 0; i < size_; i++) {
      delete data_[i];
    }
    delete[] data_;
    data_ = nullptr;
  }

private:
  char **data_;
  size_t size_;
};

int InitNode() {
  // just give a process name
  std::vector<std::string> args_vec = {"node"};
  Args args(args_vec);
  char **ret_ptr = uv_setup_args(args.size(), args.data());
  if (ret_ptr == args.data()) {
    LOGW("args copy failed in libuv\n");
  }

  std::vector<std::string> exec_args;
  std::vector<std::string> errors;

  char *title = new char[100]();
  uv_get_process_title(title, 100);
  LOGD("[jujube check] first check title is %s\n", title);
  delete[] title;

  int exit_code = node::InitializeNodeWithArgs(&args_vec, &exec_args, &errors);
  for (const std::string &error : errors) {
    LOGE("init node with args: { %s } failed, error msg: %s\n", args[0],
         error.c_str());
  }
  if (exit_code != 0) {
    return exit_code;
  }

  title = new char[100]();
  uv_get_process_title(title, 100);
  LOGD("[jujube check] second check title is %s\n", title);
  delete[] title;

  node_platform = node::MultiIsolatePlatform::Create(4);
  v8::V8::InitializePlatform(node_platform.get());
  v8::V8::Initialize();
  int ret = RunNodeInstance(node_platform.get(), args_vec, exec_args);

  v8::V8::Dispose();
  v8::V8::ShutdownPlatform();
  return ret;
}

static int RunNodeInstance(node::MultiIsolatePlatform *platform,
                           const std::vector<std::string> &args,
                           const std::vector<std::string> &exec_args) {
  int exit_code;
  uv_loop_t loop;
  int ret = uv_loop_init(&loop);
  if (ret != 0) {
    LOGE("Create uv loop failed!\n");
    return ret;
  }

  std::shared_ptr<node::ArrayBufferAllocator> allocator =
      node::ArrayBufferAllocator::Create();

  v8::Isolate *isolate = node::NewIsolate(allocator, &loop, platform);
  if (!isolate) {
    LOGE("Create isolate failed\n");
    return ISOLATE_CREATE_FAILED;
  }

  {
    v8::Locker locker(isolate);
    v8::Isolate::Scope isolate_scope(isolate);
    std::unique_ptr<node::IsolateData, decltype(&node::FreeIsolateData)>
        isolate_data(
            node::CreateIsolateData(isolate, &loop, platform, allocator.get()),
            node::FreeIsolateData);

    // Set up a new v8::Context.
    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::Context> context = node::NewContext(isolate);
    if (context.IsEmpty()) {
      fprintf(stderr, "%s: Failed to initialize V8 Context\n", args[0].c_str());
      return 1;
    }

    // The v8::Context needs to be entered when node::CreateEnvironment() and
    // node::LoadEnvironment() are being called.
    v8::Context::Scope context_scope(context);

    // Create a node::Environment instance that will later be released using
    // node::FreeEnvironment().
    std::unique_ptr<node::Environment, decltype(&node::FreeEnvironment)> env(
        node::CreateEnvironment(isolate_data.get(), context, args, exec_args),
        node::FreeEnvironment);

    v8::MaybeLocal<v8::Value> loadenv_ret = node::LoadEnvironment(
        env.get(), "const publicRequire ="
                   "  require('module').createRequire(process.cwd() + '/');"
                   "globalThis.require = publicRequire;"
                   "require('vm').runInThisContext(process.argv[1]);");
    if (loadenv_ret.IsEmpty()) // There has been a JS exception.
    {
      return 1;
    }
    {
      v8::SealHandleScope seal(isolate);
      bool more;
      do {
        uv_run(&loop, UV_RUN_DEFAULT);

        platform->DrainTasks(isolate);

        more = uv_loop_alive(&loop);
        if (more) {
          continue;
        }

        node::EmitBeforeExit(env.get());

        more = uv_loop_alive(&loop);

      } while (more);

      // node::EmitExit() returns the current exit code.
      exit_code = node::EmitExit(env.get());

      // node::Stop() can be used to explicitly stop the event loop and keep
      // further JavaScript from running. It can be called from any thread,
      // and will act like worker.terminate() if called from another thread.
      node::Stop(env.get());
    }
  }
  bool platform_finished = false;
  platform->AddIsolateFinishedCallback(
      isolate, [](void *data) { *static_cast<bool *>(data) = true; },
      &platform_finished);
  platform->UnregisterIsolate(isolate);
  isolate->Dispose();

  // Wait until the platform has cleaned up all relevant resources.
  while (!platform_finished)
    uv_run(&loop, UV_RUN_ONCE);
  int err = uv_loop_close(&loop);
  assert(err == 0);

  return exit_code;
}