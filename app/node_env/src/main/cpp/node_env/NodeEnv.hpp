#pragma once

#include <node/node.h>
#include <node/uv.h>

#include <memory>
#include <mutex>
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

  void SpinEventLoop();
  void Pause();
  void Stop();
  void Resume();
  void Destroy();
  bool Eval(const std::string &code, std::string &result);
  bool is_pause();
  bool is_stop();
  bool is_destroy();
  static void Clear();

 public:
  struct InternalModule;

 private:
  void InnerStop();
  void InnerDestroy();
  static int PrepareUvloop(const std::vector<std::string> &vec_argv);
  static int PrepareNodeEnv(std::vector<std::string> &args);
  static void LoadInternalModules();
  static node::IsolateData *CreateNodeIsolateData();
  static node::Environment *CreateNodeEnv(
      const std::vector<std::string> &argv,
      const std::vector<std::string> &exec_argv);

  std::unique_ptr<node::MultiIsolatePlatform> platform_;
  uv_loop_t loop_;
  v8::Isolate *isolate_;
  v8::Persistent<v8::Context> context_;
  node::IsolateData *isolate_data_;
  node::Environment *node_env_;
  bool is_pause_;
  std::mutex is_pause_lock_;
  bool is_stop_;
  std::mutex is_stop_lock_;
  bool is_destroy_;
  std::mutex is_destroy_lock_;

  static std::vector<InternalModule> internal_modules_;

  static NodeEnv *instance_;
  static std::string preload_script_;
};