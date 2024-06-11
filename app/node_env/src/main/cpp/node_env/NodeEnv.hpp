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

  void Pause();

  void Resume();

  void Destroy();

  void SpinEventLoop();

  bool Eval(const std::string &code, std::string &result);

  bool is_pause();

  static NodeEnv *Create(const char *preload_script);

  static NodeEnv *Create(std::vector<std::string> vec_args = {"node"},
                         const char *preload_script = "");

  static void Clear();

private:
  void Stop();

  static int PrepareUvloop(const std::vector<std::string> &vec_argv);

  static int PrepareNodeEnv(std::vector<std::string> &args);

  static node::IsolateData *CreateNodeIsoateData();

  static node::Environment *
  CreateNodeEnv(const std::vector<std::string> &argv,
                const std::vector<std::string> &exec_argv);

  static void LoadInternalModule(const char *module_preload_script,
                                 const char *module_name,
                                 node::addon_context_register_func init_fn);

  static void LoadNapiModule(const char *module_preload_script);

  std::unique_ptr<node::MultiIsolatePlatform> platform_;
  uv_loop_t loop_;
  v8::Isolate *isolate_;
  v8::Persistent<v8::Context> context_;
  node::IsolateData *isolate_data_;
  node::Environment *node_env_;
  bool is_pause_;
  std::mutex is_pause_lock_;

  static NodeEnv *instance_;
  static std::string preload_script_;
};