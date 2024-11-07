#pragma once
#include "DelayNode.hpp"
#include "JSAudioNode.hpp"

namespace js_audio {
class JSDelayNode : public JSAudioNode {
public:
  static void Init(Napi::Env env, Napi::Object exports);

  JSDelayNode(const Napi_IH::IHCallbackInfo &info,
              std::shared_ptr<DelayNode> delay_node_ref = nullptr);

  Napi::Value getDelayTime(const Napi::CallbackInfo &info);

private:
  std::weak_ptr<DelayNode> delay_node_ptr_;
};
} // namespace js_audio