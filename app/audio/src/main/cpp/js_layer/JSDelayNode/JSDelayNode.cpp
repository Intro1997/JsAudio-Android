#include "JSDelayNode.hpp"
#include "JSAudioParam.hpp"
#include "JSBaseAudioContext.hpp"

#include <cmath>

using DelayNode = js_audio::DelayNode;
using DelayOptions = js_audio::DelayNode::DelayOptions;
using JSAudioNode = js_audio::JSAudioNode;
using JSBaseAudioContext = js_audio::JSBaseAudioContext;
template <typename T>
Napi_IH::FunctionWrapper (*FindClass)() = Napi_IH::IHObjectWrap::FindClass<T>;

static bool GetMaxDelayTimeValueFromJsOptions(Napi::Env napi_env,
                                              const Napi::Object &js_options,
                                              float &max_delay_time,
                                              Napi::Error &napi_error);

static bool GetDelayTimeValueFromJsOptions(Napi::Env napi_env,
                                           const Napi::Object &js_options,
                                           float &delay_time,
                                           Napi::Error &napi_error);

static bool ExtractOptionsFromInfo(const Napi_IH::IHCallbackInfo &info,
                                   DelayOptions &options,
                                   Napi::Error &napi_error);

static std::shared_ptr<DelayNode>
GetDelayNodeRef(const Napi_IH::IHCallbackInfo &info,
                std::shared_ptr<DelayNode> delay_node_ref);

namespace js_audio {
JSDelayNode::JSDelayNode(const Napi_IH::IHCallbackInfo &info,
                         std::shared_ptr<DelayNode> delay_node_ref)
    : JSAudioNode(info, GetDelayNodeRef(info, delay_node_ref)) {
  delay_node_ptr_ = std::static_pointer_cast<DelayNode>(audio_node_ref_);
}

void JSDelayNode::Init(Napi::Env env, Napi::Object exports) {
  DefineClass<JSDelayNode, JSAudioNode>(
      env, "DelayNode",
      {InstanceAccessor<JSDelayNode, &JSDelayNode::getDelayTime>("delayTime")});
}

Napi::Value JSDelayNode::getDelayTime(const Napi::CallbackInfo &info) {
  if (auto delay_node_ref = delay_node_ptr_.lock()) {
    Napi::Object js_delay_time =
        FindClass<JSAudioParam>().NewWithArgs<JSAudioParam>(
            {info.This()}, delay_node_ref->delay_time_ref());
    return js_delay_time;
  }
  return info.Env().Undefined();
}
}; // namespace js_audio

static bool GetMaxDelayTimeValueFromJsOptions(Napi::Env napi_env,
                                              const Napi::Object &js_options,
                                              float &max_delay_time,
                                              Napi::Error &napi_error) {
  napi_error.Reset();
  if (js_options.Has("maxDelayTime")) {
    float maybe_max_delay_time = js_options.Get("maxDelayTime").ToNumber();
    if (std::isnan(maybe_max_delay_time)) {
      napi_error = Napi::TypeError::New(
          napi_env, "Failed to construct 'DelayNode': Failed to read the "
                    "'maxDelayTime' property from 'DelayOptions': The provided "
                    "double value is non-finite.\n");
      return false;
    } else if (maybe_max_delay_time <= 0.0 || maybe_max_delay_time > 179) {
      napi_error = Napi_IH::RangeError::New(
          napi_env,
          "Failed to construct 'DelayNode': The max delay time provided (%f) "
          "is outside the range (0, 180).\n",
          maybe_max_delay_time);
      return false;
    }
    max_delay_time = maybe_max_delay_time;
  }
  return true;
}

static bool GetDelayTimeValueFromJsOptions(Napi::Env napi_env,
                                           const Napi::Object &js_options,
                                           float &delay_time,
                                           Napi::Error &napi_error) {
  napi_error.Reset();
  if (js_options.Has("delayTime")) {
    float maybe_delay_time = js_options.Get("delayTime").ToNumber();
    if (std::isnan(maybe_delay_time)) {
      napi_error = Napi::TypeError::New(
          napi_env, "Failed to construct 'DelayNode': Failed to read the "
                    "'delayTime' property from 'DelayOptions': The provided "
                    "float value is non-finite.\n");
      return false;
    }
    delay_time = maybe_delay_time;
  }
  return true;
}

bool ExtractOptionsFromInfo(const Napi_IH::IHCallbackInfo &info,
                            DelayOptions &options, Napi::Error &napi_error) {
  if (info.Length() < 2 || info[1].IsUndefined() || info[1].IsNull()) {
    return true;
  }
  if (!info[1].IsObject()) {
    napi_error = Napi::TypeError::New(
        info.Env(), "Failed to construct 'DelayNode': The provided value is "
                    "not of type 'DelayOptions'.\n");
    return false;
  }

  Napi::Env env = info.Env();
  Napi::Object js_options = info[1].ToObject();
  if (!JSAudioNode::ExtractOptionsFromInfo("Delay", info, options,
                                           napi_error)) {
    return false;
  }
  if (!GetMaxDelayTimeValueFromJsOptions(env, js_options,
                                         options.max_delay_time, napi_error)) {
    return false;
  }
  if (!GetDelayTimeValueFromJsOptions(env, js_options, options.delay_time,
                                      napi_error)) {
    return false;
  }

  float &delay_time = options.delay_time;
  float &max_delay_time = options.max_delay_time;
  if (delay_time < 0 || delay_time > max_delay_time) {
    delay_time = std::clamp(delay_time, 0.0f, max_delay_time);
    LOGW("Delay.delayTime.value %f outside nominal range [0, %f]; value will "
         "be clamped.\n",
         delay_time, max_delay_time);
  }
  return true;
}

static std::shared_ptr<DelayNode>
GetDelayNodeRef(const Napi_IH::IHCallbackInfo &info,
                std::shared_ptr<DelayNode> delay_node_ref) {
  if (delay_node_ref) {
    return delay_node_ref;
  }
  if (info.Length() < 1) {
    throw Napi_IH::TypeError::New(info.Env(),
                                  "Failed to construct 'DelayNode': 1 argument "
                                  "required, but only 0 present.");
  }
  if (!info[0].IsObject()) {
    throw Napi::TypeError::New(info.Env(),
                               "Failed to construct 'DelayNode': parameter 1 "
                               "is not of type 'BaseAudioContext'.\n");
  }

  Napi::Object js_base_audio_context = info[0].As<Napi::Object>();
  if (!js_base_audio_context.InstanceOf(
          FindClass<JSBaseAudioContext>().InnerFunction())) {
    throw Napi::TypeError::New(info.Env(),
                               "Failed to construct 'DelayNode': parameter 1 "
                               "is not of type 'BaseAudioContext'.\n");
  }

  Napi::Error napi_error;
  DelayOptions options = DelayNode::GetDefaultOptions();

  if (!ExtractOptionsFromInfo(info, options, napi_error)) {
    throw napi_error;
  }

  JSBaseAudioContext *js_base_audio_context_ptr =
      Napi_IH::IHObjectWrap::UnWrap<JSBaseAudioContext>(js_base_audio_context);
  std::shared_ptr<std::mutex> audio_context_lock_ref =
      js_base_audio_context_ptr->GetAudioContextLock();

  return DelayNode::CreateDelayNode(options,
                                    js_base_audio_context_ptr->GetSampleRate(),
                                    audio_context_lock_ref);
}