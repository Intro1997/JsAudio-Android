#pragma once

#include <memory>
#include <mutex>
#include <node/node.h>
#include <node/uv.h>
#include <string>
#include <vector>

class NodeEnv {
public:
  NodeEnv();
  ~NodeEnv();

  static void AddInternalModule(const char *module_preload_script,
                                const char *module_name,
                                node::addon_context_register_func init_fn);
  static NodeEnv *Create(const char *preload_script);
  static NodeEnv *Create(std::vector<std::string> vec_args = {"node"},
                         const char *preload_script = "");

  void Pause();
  void Resume();
  void Destroy();
  void SpinEventLoop();
  bool Eval(const std::string &code, std::string &result);
  bool is_pause();
  static void Clear();

public:
  struct InternalModule;

private:
  void Stop();

  static int PrepareUvloop(const std::vector<std::string> &vec_argv);
  static int PrepareNodeEnv(std::vector<std::string> &args);
  static void LoadInternalModules();
  static node::IsolateData *CreateNodeIsolateData();
  static node::Environment *
  CreateNodeEnv(const std::vector<std::string> &argv,
                const std::vector<std::string> &exec_argv);

  std::unique_ptr<node::MultiIsolatePlatform> platform_;
  uv_loop_t loop_;
  v8::Isolate *isolate_;
  v8::Persistent<v8::Context> context_;
  node::IsolateData *isolate_data_;
  node::Environment *node_env_;
  bool is_pause_;
  std::mutex is_pause_lock_;

  static std::vector<InternalModule> internal_modules_;

  static NodeEnv *instance_;
  static std::string preload_script_;
};