#pragma once
#include <memory>
#include <mutex>
#include <node/node.h>
#include <node/uv.h>
#include <string>
#include <vector>

class NodeInstance {
public:
  NodeInstance() = default;
  ~NodeInstance();

  void Pause();
  void Resume();

  void Destroy();
  void SpinEventLoop();

  bool Eval(const std::string &code, std::string &result);

  bool is_pause();
  static NodeInstance *Create(std::vector<std::string> vec_args = {"node"});
  static void Clear();

private:
  void Stop();
  static int PrepareUvloop(const std::vector<std::string> &vec_argv);
  static int PrepareNodeEnv(const std::vector<std::string> &args,
                            const std::vector<std::string> &exec_args);
  static int PrepareNodeEnvData();
  static void PrepareInternalModule();
  static int CreateNodeEnv(const std::vector<std::string> &argv,
                           const std::vector<std::string> &exec_argv);

  std::unique_ptr<node::MultiIsolatePlatform> platform_;
  uv_loop_t loop_;
  v8::Isolate *isolate_;
  v8::Persistent<v8::Context> context_;
  node::IsolateData *isolate_data_;
  node::Environment *node_env_;
  bool is_pause_;
  std::mutex is_pause_lock_;

  static NodeInstance *instance_;
};