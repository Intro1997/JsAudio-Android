#pragma once
#include "JSAudioScheduledSourceNode.hpp"
#include "OscillatorNode.hpp"
namespace js_audio {
class JSOscillatorNode : public JSAudioScheduledSourceNode {
public:
  static void Init(Napi::Env env, Napi::Object exports);

  JSOscillatorNode(
      const Napi_IH::IHCallbackInfo &info,
      std::shared_ptr<OscillatorNode> oscillator_node_ref = nullptr);

private:
  void setType(const Napi::CallbackInfo &info, const Napi::Value &value);

  Napi::Value getFrequency(const Napi::CallbackInfo &info);
  Napi::Value getDetune(const Napi::CallbackInfo &info);
  Napi::Value getType(const Napi::CallbackInfo &info);

  std::weak_ptr<OscillatorNode> oscillator_node_ptr_;
};
} // namespace js_audio
