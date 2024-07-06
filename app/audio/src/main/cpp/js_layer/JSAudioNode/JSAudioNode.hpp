#pragma once
#include <napi_ih/napi_ih.hpp>
namespace js_audio {
class JSAudioNode : public Napi_IH::IHObjectWrap {
public:
  JSAudioNode(const Napi_IH::IHCallbackInfo &info);
  static void Init(Napi::Env env, Napi::Object exports);

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