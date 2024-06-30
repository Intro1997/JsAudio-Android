#pragma once
#include <napi/napi.h>
namespace js_audio {
class JSAudioNode : public Napi::ObjectWrap<JSAudioNode> {
public:
  JSAudioNode(const Napi::CallbackInfo &info);
  static void Init(Napi::Env env, Napi::Object exports);
  static Napi::Function GetClass(Napi::Env env);

private:
  Napi::Value GetNumberOfInputs(const Napi::CallbackInfo &info);
  Napi::Value GetNumberOfOutputs(const Napi::CallbackInfo &info);
  Napi::Value GetChannelCount(const Napi::CallbackInfo &info);

  static Napi::FunctionReference
      js_audio_node_class_ref_; // constructor of class
  uint32_t number_of_inputs_;
  uint32_t number_of_outputs_;
  uint32_t channel_count_;
};
} // namespace js_audio