#pragma once
#include <functional>
#include <napi_ih/napi_ih.hpp>
#include <string>

#include "AudioParam.hpp"

namespace js_audio {
class JSAudioParam : public Napi_IH::IHObjectWrap {
public:
  using SetterCallbackFunc = void(const float &);
  using GetterCallbackFunc = void();

  JSAudioParam(const Napi_IH::IHCallbackInfo &info,
               std::shared_ptr<AudioParam> audio_param_ref = nullptr);

  static void Init(Napi::Env env, Napi::Object exports);

  void setValue(const Napi::CallbackInfo &info, const Napi::Value &value);
  Napi::Value getValue(const Napi::CallbackInfo &info);
  Napi::Value getAutomationRate(const Napi::CallbackInfo &info);
  Napi::Value getDefaultValue(const Napi::CallbackInfo &info);
  Napi::Value getMinValue(const Napi::CallbackInfo &info);
  Napi::Value getMaxValue(const Napi::CallbackInfo &info);

private:
  std::shared_ptr<AudioParam> audio_param_ref_;
};

} // namespace js_audio
