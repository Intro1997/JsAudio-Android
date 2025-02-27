#include "NodeEnv.hpp"

#include <node/node.h>
#include <node/uv.h>

#include "Argv.hpp"
#include "logger.hpp"
#include "preload_script.hpp"
#include "sleeper.hpp"

#define NI_SUCCESS 0
#define NI_NODE_CREATE_ISOLATE_DATA_FAILED 1
#define NI_NODE_CREATE_ENV_FAILED 2
#define NI_UV_LOOP_INIT_FAILED 3
#define NI_NODE_INITIALIZE_WITH_ARGS_FAILED 4

struct NodeEnv::InternalModule {
  const char *module_preload_script;
  const char *module_name;
  node::addon_context_register_func init_fn;
};

NodeEnv *NodeEnv::instance_ = nullptr;
std::string NodeEnv::preload_script_ = NODE_INSTANCE_PRELOAD_SCRIPT;
std::vector<NodeEnv::InternalModule> NodeEnv::internal_modules_;

NodeEnv::NodeEnv() : is_pause_(false) {}

NodeEnv::~NodeEnv() {
  if (platform_) {
    this->InnerStop();
    this->InnerDestroy();
  }
}

void NodeEnv::AddInternalModule(const char *module_preload_script,
                                const char *module_name,
                                node::addon_context_register_func init_fn) {
  InternalModule internal_module;
  internal_module.module_preload_script = module_preload_script;
  internal_module.module_name = module_name;
  internal_module.init_fn = init_fn;
  internal_modules_.push_back(internal_module);
}

NodeEnv *NodeEnv::Create(const char *preload_script) {
  return Create({"node", "--expose-gc"}, preload_script);
}

NodeEnv *NodeEnv::Create(std::vector<std::string> vec_argv,
                         const char *preload_script) {
  if (instance_ != nullptr) {
    return instance_;
  }

  instance_ = new NodeEnv();

  instance_->preload_script_ += preload_script;

  if (PrepareUvloop(vec_argv) != NI_SUCCESS) {
    LOGE("Prepare uv loop failed!\n");
    return nullptr;
  }

  if (PrepareNodeEnv(vec_argv) != NI_SUCCESS) {
    LOGE("Prepare node env failed!\n");
    NodeEnv::Clear();
    return nullptr;
  }

  return instance_;
}

bool NodeEnv::is_pause() {
  std::lock_guard<std::mutex> guard(is_pause_lock_);
  return is_pause_;
}

bool NodeEnv::is_stop() {
  std::lock_guard<std::mutex> guard(is_stop_lock_);
  return is_stop_;
}

bool NodeEnv::is_destroy() {
  std::lock_guard<std::mutex> guard(is_destroy_lock_);
  return is_stop_;
}

void NodeEnv::Pause() {
  LOGD("Pause Node Instance!");
  std::lock_guard<std::mutex> guard(is_pause_lock_);
  if (!is_pause_) {
    is_pause_ = true;
  }
}

void NodeEnv::Resume() {
  LOGD("Resume Node Instance!");
  std::lock_guard<std::mutex> guard(is_pause_lock_);
  if (is_pause_) {
    is_pause_ = false;
  }
}

void NodeEnv::Stop() {
  LOGD("Stop Node Instance!");
  std::lock_guard<std::mutex> guard(is_stop_lock_);
  if (!is_stop_) {
    is_stop_ = true;
  }
}

void NodeEnv::Destroy() {
  LOGD("Destroy Node Instance!");
  std::lock_guard<std::mutex> guard(is_stop_lock_);
  if (!is_stop_) {
    is_stop_ = true;
  }
}

void NodeEnv::InnerStop() {
  if (node_env_) {
    v8::Locker locker(isolate_);
    v8::Isolate::Scope isolate_scope(isolate_);
    node::EmitProcessExit(node_env_);
    node::Stop(node_env_);
  }
}

void NodeEnv::InnerDestroy() {
  preload_script_ = NODE_INSTANCE_PRELOAD_SCRIPT;

  if (!instance_) {
    return;
  }

  internal_modules_.clear();
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

void NodeEnv::SpinEventLoop() {
  // you must call this methods after
  // create v8::Locker and v8::Isolate::Scope!
  {
    v8::SealHandleScope seal(instance_->isolate_);
    bool more;
    do {
      if (is_stop() || is_destroy()) {
        InnerStop();
        break;
      }
      if (is_destroy()) {
        InnerDestroy();
        break;
      }
      while (is_pause()) {
        sleep_for_ms(16);
      }

      uv_run(&instance_->loop_, UV_RUN_ONCE);

      instance_->platform_->DrainTasks(instance_->isolate_);

      more = uv_loop_alive(&instance_->loop_);
      if (more) {
        continue;
      }

      node::EmitProcessBeforeExit(instance_->node_env_);

      more = uv_loop_alive(&instance_->loop_);

    } while (more);
  }
}

int NodeEnv::PrepareUvloop(const std::vector<std::string> &vec_argv) {
  Argv argv(vec_argv);
  char **argv_cpy = uv_setup_args(argv.count(), argv.get());
  if (argv_cpy == argv.get()) {
    LOGW("argv copy failed in libuv\n");
  }
  int exit_code;
  exit_code = uv_loop_init(&instance_->loop_);

  if (exit_code != NI_SUCCESS) {
    LOGE("Init uv loop failed\n");
    return NI_UV_LOOP_INIT_FAILED;
  }
  return exit_code;
}

int NodeEnv::PrepareNodeEnv(std::vector<std::string> &argv) {
  std::vector<std::string> exec_argv;

  std::unique_ptr<node::InitializationResult> result =
      node::InitializeOncePerProcess(
          argv,
          {node::ProcessInitializationFlags::kNoInitializeV8,
           node::ProcessInitializationFlags::kNoInitializeNodeV8Platform});

  auto &ret_errors = result->errors();
  if (!ret_errors.empty()) {
    LOGE("Init NodeJs failed!\n");
    for (const std::string &error : ret_errors)
      LOGE("%s: %s\n", argv[0].c_str(), error.c_str());
    if (result->early_return() != 0) {
      LOGE("Error with early return.\n");
      return NI_NODE_INITIALIZE_WITH_ARGS_FAILED;
    }
  }

  instance_->platform_ = node::MultiIsolatePlatform::Create(4);
  v8::V8::InitializePlatform(instance_->platform_.get());
  v8::V8::Initialize();

  node::IsolateData *isolate_data = CreateNodeIsolateData();
  if (isolate_data == nullptr) {
    LOGE("Create node isolate data failed!\n");
    return NI_NODE_CREATE_ISOLATE_DATA_FAILED;
  }

  node::Environment *node_env = CreateNodeEnv(argv, exec_argv);
  if (node_env == nullptr) {
    LOGE("Create node env failed!\n");
    return NI_NODE_CREATE_ENV_FAILED;
  }
  return NI_SUCCESS;
}

void NodeEnv::Clear() {
  if (instance_ != nullptr) {
    delete instance_;
    instance_ = nullptr;
  }
}

node::IsolateData *NodeEnv::CreateNodeIsolateData() {
  uv_loop_t &loop = instance_->loop_;
  node::MultiIsolatePlatform *platform = instance_->platform_.get();

  std::shared_ptr<node::ArrayBufferAllocator> allocator_ref =
      node::ArrayBufferAllocator::Create();

  instance_->isolate_ = node::NewIsolate(allocator_ref, &loop, platform);

  if (!instance_->isolate_) {
    LOGE("Create isolate failed\n");
    return nullptr;
  }

  instance_->isolate_data_ = node::CreateIsolateData(
      instance_->isolate_, &loop, platform, allocator_ref.get());
  if (!instance_->isolate_data_) {
    LOGE("Create isolate data failed\n");
    return nullptr;
  }
  return instance_->isolate_data_;
}

void NodeEnv::LoadInternalModules() {
  if (!instance_->node_env_) {
    LOGE("Prepare internal module failed! Node env is invaild!\n");
    return;
  }

  for (const auto &module : internal_modules_) {
    preload_script_ += module.module_preload_script;
    node::AddLinkedBinding(instance_->node_env_, module.module_name,
                           module.init_fn, nullptr);
  }
}

node::Environment *
NodeEnv::CreateNodeEnv(const std::vector<std::string> &argv,
                       const std::vector<std::string> &exec_argv) {
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
      return nullptr;
    }
    instance_->context_.Reset(isolate, context);

    // The v8::Context needs to be entered when node::CreateEnvironment() and
    // node::LoadEnvironment() are being called.
    v8::Context::Scope context_scope(context);

    // Create a node::Environment instance that will later be released using
    // node::FreeEnvironment().
    node::Environment *env = instance_->node_env_ =
        node::CreateEnvironment(isolate_data, context, argv, exec_argv);

    LoadInternalModules();
    internal_modules_.clear();

    v8::TryCatch trycatch(isolate);
    v8::MaybeLocal<v8::Value> loadenv_ret =
        node::LoadEnvironment(env, preload_script_.c_str());
    if (loadenv_ret.IsEmpty()) {
      std::string err_msg = "";
      if (trycatch.HasCaught()) {
        v8::Local<v8::Value> exception = trycatch.Exception();
        v8::String::Utf8Value message(isolate, exception);
        err_msg = "msg: ";
        err_msg += *message;
      }
      LOGE("Load nodejs env failed! %s\n", err_msg.c_str());
      return nullptr;
    }
    instance_->SpinEventLoop();
  }
  return instance_->node_env_;
}

bool NodeEnv::Eval(const std::string &code, std::string &result) {
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
  v8::TryCatch trycatch(isolate);

  v8::MaybeLocal<v8::Script> maybe_script = v8::Script::Compile(
      context, v8::String::NewFromUtf8(isolate, code.c_str()).ToLocalChecked());

  if (!maybe_script.IsEmpty()) {
    v8::Local<v8::Script> code_script = maybe_script.ToLocalChecked();
    v8::MaybeLocal<v8::Value> maybe_ret = code_script->Run(context);
    if (!maybe_ret.IsEmpty()) {
      instance_->SpinEventLoop();
      return true;
    }
  }

  if (trycatch.HasCaught()) {
    v8::Local<v8::Value> exception = trycatch.Exception();
    v8::String::Utf8Value exception_message(isolate, exception);
    result = *exception_message;
    return false;
  } else {
    result = "unknown error\n";
    return false;
  }

  return true;
}