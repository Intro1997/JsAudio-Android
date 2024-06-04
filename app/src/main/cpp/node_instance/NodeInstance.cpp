#include "NodeInstance.hpp"
#include <node/node.h>
#include <node/uv.h>

#include "Argv.hpp"
#include "logger.hpp"
#include "node_logger.hpp"
#include "sleeper.hpp"

#define NI_SUCCESS 0
#define NI_V8_CREATE_ISOLATE_FAILED 1
#define NI_V8_CREATE_ISOLATE_DATA_FAILED 2
#define NI_V8_INIT_CONTEXT_FAILED 3
#define NI_LOAD_NODE_ENV_FAILED 4
#define NI_UV_LOOP_INIT_FAILED 5

NodeInstance *NodeInstance::instance_ = nullptr;

NodeInstance::~NodeInstance() {
  if (platform_) {
    this->Stop();
    this->Destroy();
  }
}

bool NodeInstance::is_pause() {
  std::lock_guard<std::mutex> guard(is_pause_lock_);
  return is_pause_;
}

void NodeInstance::Pause() {
  LOGD("Pause Node Instance!");
  std::lock_guard<std::mutex> guard(is_pause_lock_);
  if (!is_pause_) {
    is_pause_ = true;
  }
}

void NodeInstance::Resume() {
  LOGD("Resume Node Instance!");
  std::lock_guard<std::mutex> guard(is_pause_lock_);
  if (is_pause_) {
    is_pause_ = false;
  }
}

void NodeInstance::Stop() {
  if (node_env_) {
    v8::Locker locker(isolate_);
    v8::Isolate::Scope isolate_scope(isolate_);
    node::EmitExit(node_env_);
    node::Stop(node_env_);
  }
}

void NodeInstance::Destroy() {
  if (isolate_) {
    v8::Locker locker(isolate_);
    v8::Isolate::Scope isolate_scope(isolate_);
    if (node_env_) {
      node::FreeEnvironment(node_env_);
    }
    context_.Reset();
  }
  if (isolate_data_) {
    node::FreeIsolateData(isolate_data_);
  }

  v8::Isolate *isolate = instance_->isolate_;
  uv_loop_t &loop = instance_->loop_;
  node::MultiIsolatePlatform *platform = instance_->platform_.get();

  if (isolate && uv_loop_alive(&loop)) {
    bool platform_finished = false;
    platform->AddIsolateFinishedCallback(
        isolate, [](void *data) { *static_cast<bool *>(data) = true; },
        &platform_finished);
    platform->UnregisterIsolate(isolate);
    isolate->Dispose();

    // Wait until the platform has cleaned up all relevant resources.
    while (!platform_finished) { // intro: access platform_finished here raises
      // thread unsafe here, atomic_bool is better
      uv_run(&loop, UV_RUN_ONCE);
    }
    int err = uv_loop_close(&loop);
    assert(err == 0);
  }

  v8::V8::Dispose();
  v8::V8::ShutdownPlatform();
  platform_.reset();
}

void NodeInstance::SpinEventLoop() {
  // you must call this methods after
  // create v8::Locker and v8::Isolate::Scope!
  {
    v8::SealHandleScope seal(instance_->isolate_);
    bool more;
    do {
      while (is_pause()) {
        sleep_for_ms(16);
      }

      uv_run(&instance_->loop_, UV_RUN_ONCE);

      instance_->platform_->DrainTasks(instance_->isolate_);

      more = uv_loop_alive(&instance_->loop_);
      if (more) {
        continue;
      }

      node::EmitBeforeExit(instance_->node_env_);

      more = uv_loop_alive(&instance_->loop_);

    } while (more);
  }
}

NodeInstance *NodeInstance::Create(std::vector<std::string> vec_argv) {
  if (instance_ != nullptr) {
    return instance_;
  }

  Argv argv(vec_argv);
  char **argv_cpy = uv_setup_args(argv.count(), argv.get());
  if (argv_cpy == argv.get()) {
    LOGW("argv copy failed in libuv\n");
  }

  std::vector<std::string> exec_argv;
  std::vector<std::string> errors;

  if (node::InitializeNodeWithArgs(&vec_argv, &exec_argv, &errors) !=
      NI_SUCCESS) {
    LOGE("Init NodeJs failed!\n");
    for (const auto &e : errors) {
      LOGE("%s\n", e.c_str());
    }
    return nullptr;
  }

  instance_ = new NodeInstance();
  instance_->is_pause_ = false;
  instance_->platform_ = node::MultiIsolatePlatform::Create(4);
  v8::V8::InitializePlatform(instance_->platform_.get());
  v8::V8::Initialize();

  if (PrepareNodeEnv(vec_argv, exec_argv) != NI_SUCCESS) {
    LOGE("Prepare node env failed!\n");
    NodeInstance::Clear();
    return nullptr;
  }

  return instance_;
}

int NodeInstance::PrepareNodeEnv(const std::vector<std::string> &argv,
                                 const std::vector<std::string> &exec_argv) {
  int exit_code = NI_SUCCESS;
  if ((exit_code = PrepareNodeEnvData()) != NI_SUCCESS) {
    LOGE("PrepareNodeEnvData failed!\n");
    return exit_code;
  }
  if ((exit_code = CreateNodeEnv(argv, exec_argv)) != NI_SUCCESS) {
    LOGE("CreateNodeEnv failed!\n");
    return exit_code;
  }
  return exit_code;
}

void NodeInstance::Clear() {
  if (instance_ != nullptr) {
    delete instance_;
    instance_ = nullptr;
  }
}

int NodeInstance::PrepareNodeEnvData() {
  int exit_code;
  exit_code = uv_loop_init(&instance_->loop_);
  if (exit_code != NI_SUCCESS) {
    LOGE("Init uv loop failed\n");
    return NI_UV_LOOP_INIT_FAILED;
  }
  uv_loop_t &loop = instance_->loop_;

  node::MultiIsolatePlatform *platform = instance_->platform_.get();

  std::shared_ptr<node::ArrayBufferAllocator> allocator =
      node::ArrayBufferAllocator::Create();

  instance_->isolate_ = node::NewIsolate(allocator, &loop, platform);

  if (!instance_->isolate_) {
    LOGE("Create isolate failed\n");
    return NI_V8_CREATE_ISOLATE_FAILED;
  }

  instance_->isolate_data_ = node::CreateIsolateData(instance_->isolate_, &loop,
                                                     platform, allocator.get());
  if (!instance_->isolate_data_) {
    LOGE("Create isolate data failed\n");
    return NI_V8_CREATE_ISOLATE_DATA_FAILED;
  }
  return exit_code;
}

void NodeInstance::PrepareInternalModule() {
  if (!instance_->node_env_) {
    LOGE("Prepare internal module failed! Node env is invaild!\n");
  }
  node::AddLinkedBinding(instance_->node_env_, "node_basic", node_logger::init,
                         NULL);
}

int NodeInstance::CreateNodeEnv(const std::vector<std::string> &argv,
                                const std::vector<std::string> &exec_argv) {
  int exit_code = NI_SUCCESS;
  v8::Isolate *isolate = instance_->isolate_;
  node::IsolateData *isolate_data = instance_->isolate_data_;

  {
    v8::Locker locker(isolate);
    v8::Isolate::Scope isolate_scope(isolate);

    // Set up a new v8::Context.
    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::Context> context = node::NewContext(isolate);
    if (context.IsEmpty()) {
      LOGE("Failed to initialize V8 Context\n");
      return NI_V8_INIT_CONTEXT_FAILED;
    }
    instance_->context_.Reset(isolate, context);

    // The v8::Context needs to be entered when node::CreateEnvironment() and
    // node::LoadEnvironment() are being called.
    v8::Context::Scope context_scope(context);

    // Create a node::Environment instance that will later be released using
    // node::FreeEnvironment().
    node::Environment *env = instance_->node_env_ =
        node::CreateEnvironment(isolate_data, context, argv, exec_argv);

    PrepareInternalModule();

    v8::TryCatch trycatch(isolate);
    v8::MaybeLocal<v8::Value> loadenv_ret = node::LoadEnvironment(
        env,
        "const publicRequire ="
        "  require('module').createRequire(process.cwd() + '/');"
        "globalThis.require = publicRequire;"
        "require('vm').runInThisContext(process.argv[1]);"
        "const node_logger = process._linkedBinding('node_basic').node_logger;"
        "globalThis.node_logger = node_logger;");
    if (loadenv_ret.IsEmpty()) {
      // There has been a JS exception.
      LOGE("Load nodejs env failed!\n");
      if (trycatch.HasCaught()) {
        v8::Local<v8::Value> exception = trycatch.Exception();
        v8::String::Utf8Value message(isolate, exception);
        LOGE("Load node env failed: %s", *message);
      }
      return NI_LOAD_NODE_ENV_FAILED;
    }
    instance_->SpinEventLoop();
  }
  return exit_code;
}

bool NodeInstance::Eval(const std::string &code, std::string &result) {
  if (!instance_ || !instance_->isolate_) {
    result = "Error: evaluate code failed, invalid node env!";
    return false;
  }

  v8::Isolate *isolate = instance_->isolate_;

  v8::Locker locker(isolate);
  v8::Isolate::Scope isolate_scope(isolate);
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Context> context = instance_->context_.Get(isolate);
  v8::Context::Scope context_scope(context);

  v8::Local<v8::Script> code_script =
      v8::Script::Compile(
          context,
          v8::String::NewFromUtf8(isolate, code.c_str()).ToLocalChecked())
          .ToLocalChecked();
  if (code_script.IsEmpty()) {
    LOGE("Compile js script failed\n");
    return false;
  }

  v8::TryCatch trycatch(isolate);
  v8::MaybeLocal<v8::Value> maybe_ret = code_script->Run(context);
  if (maybe_ret.IsEmpty()) {
    if (trycatch.HasCaught()) {
      v8::Local<v8::Value> exception = trycatch.Exception();
      v8::String::Utf8Value exception_message(isolate, exception);
      result = *exception_message;
      return false;
    } else {
      result = "";
    }
  }
  instance_->SpinEventLoop();
  return true;
}