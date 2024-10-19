#include "JSAudioParam.hpp"
namespace js_audio {

JSAudioParam::JSAudioParam(const Napi_IH::IHCallbackInfo &info,
                           std::shared_ptr<AudioParam> audio_param_ref)
    : Napi_IH::IHObjectWrap(info), audio_param_ref_(audio_param_ref) {}

void JSAudioParam::Init(Napi::Env env, Napi::Object exports) {
  DefineClass<JSAudioParam>(
      env, "AudioParam",
      {InstanceAccessor<JSAudioParam, &JSAudioParam::getValue,
                        &JSAudioParam::setValue>("value"),
       InstanceAccessor<JSAudioParam, &JSAudioParam::getAutomationRate>(
           "automationRate"),
       InstanceAccessor<JSAudioParam, &JSAudioParam::getDefaultValue>(
           "defaultValue"),
       InstanceAccessor<JSAudioParam, &JSAudioParam::getMinValue>("minValue"),
       InstanceAccessor<JSAudioParam, &JSAudioParam::getMaxValue>("maxValue")},
      Napi_IH::ClassVisibility::kHideConstructor);
}

void JSAudioParam::setValue(const Napi::CallbackInfo &info,
                            const Napi::Value &value) {
  if (!value.IsNumber()) {
    LOGE("Error: Invalid AudioParam Value! Value must be number!\n");
  }

  if (audio_param_ref_) {
    float new_value = value.As<Napi::Number>().operator float();
    audio_param_ref_->set_value(new_value);
  }
}

Napi::Value JSAudioParam::getValue(const Napi::CallbackInfo &info) {
  if (!audio_param_ref_) {
    return info.Env().Undefined();
  }

  Napi::Number value = Napi::Number::New(info.Env(), audio_param_ref_->value());
  return value;
}

Napi::Value JSAudioParam::getAutomationRate(const Napi::CallbackInfo &info) {
  if (!audio_param_ref_) {
    return info.Env().Undefined();
  }
  Napi::String automation_rate =
      Napi::String::New(info.Env(), audio_param_ref_->automation_rate());
  return automation_rate;
}

Napi::Value JSAudioParam::getDefaultValue(const Napi::CallbackInfo &info) {
  if (!audio_param_ref_) {
    return info.Env().Undefined();
  }
  Napi::Number default_value =
      Napi::Number::New(info.Env(), audio_param_ref_->default_value());
  return default_value;
}

Napi::Value JSAudioParam::getMinValue(const Napi::CallbackInfo &info) {
  if (!audio_param_ref_) {
    return info.Env().Undefined();
  }
  Napi::Number min_value =
      Napi::Number::New(info.Env(), audio_param_ref_->min_value());
  return min_value;
}

Napi::Value JSAudioParam::getMaxValue(const Napi::CallbackInfo &info) {
  if (!audio_param_ref_) {
    return info.Env().Undefined();
  }
  Napi::Number max_value =
      Napi::Number::New(info.Env(), audio_param_ref_->max_value());
  return max_value;
}

} // namespace js_audio
