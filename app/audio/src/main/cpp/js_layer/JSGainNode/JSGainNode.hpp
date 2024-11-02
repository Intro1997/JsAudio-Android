#pragma once
#include "GainNode.hpp"
#include "JSAudioNode.hpp"

namespace js_audio {
class JSGainNode : public JSAudioNode {
public:
  JSGainNode(const Napi_IH::IHCallbackInfo &info,
             std::shared_ptr<GainNode> gain_node_ref = nullptr);
  static void Init(Napi::Env env, Napi::Object exports);

  Napi::Value getGain(const Napi::CallbackInfo &info);

private:
  std::weak_ptr<GainNode> gain_node_ptr_;
};

} // namespace js_audio
