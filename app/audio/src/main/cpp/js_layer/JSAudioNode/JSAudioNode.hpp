#pragma once
#include "AudioNode.hpp"
#include <napi_ih/napi_ih.hpp>
namespace js_audio {
class JSAudioNode : public Napi_IH::IHObjectWrap {
public:
  JSAudioNode(const Napi_IH::IHCallbackInfo &info,
              std::shared_ptr<AudioNode> audio_destination_node_ptr = nullptr);
  static void Init(Napi::Env env, Napi::Object exports);

protected:
  std::shared_ptr<AudioNode> audio_node_ptr_;

private:
  Napi::Value getContext(const Napi::CallbackInfo &info);
  Napi::Value getNumberOfInputs(const Napi::CallbackInfo &info);
  Napi::Value getNumberOfOutputs(const Napi::CallbackInfo &info);
  Napi::Value getChannelCount(const Napi::CallbackInfo &info);
  Napi::Value getChannelCountMode(const Napi::CallbackInfo &info);

  Napi::Value connect(const Napi::CallbackInfo &info);
  Napi::Value disconnect(const Napi::CallbackInfo &info);

  bool IsSameAudioContext(const JSAudioNode * other_audio_node);

  // weak ref
  Napi::ObjectReference napi_audio_context_ref_;
};
} // namespace js_audio