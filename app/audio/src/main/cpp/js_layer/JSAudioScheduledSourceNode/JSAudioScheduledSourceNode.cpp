#include "JSAudioScheduledSourceNode.hpp"
#include <cmath>

namespace js_audio {
JSAudioScheduledSourceNode::JSAudioScheduledSourceNode(
    const Napi_IH::IHCallbackInfo &info,
    std::shared_ptr<AudioScheduledSourceNode> audio_scheduled_source_node_ref)
    : JSAudioNode(info, audio_scheduled_source_node_ref) {}

void JSAudioScheduledSourceNode::Init(Napi::Env env, Napi::Object exports) {
  DefineClass<JSAudioScheduledSourceNode, JSAudioNode>(
      env, "AudioScheduledSourceNode",
      {InstanceMethod<JSAudioScheduledSourceNode,
                      &JSAudioScheduledSourceNode::Start>("start"),
       InstanceMethod<JSAudioScheduledSourceNode,
                      &JSAudioScheduledSourceNode::Stop>("stop")},
      Napi_IH::ClassVisibility::kHideConstructor);
}

Napi::Value JSAudioScheduledSourceNode::Start(const Napi::CallbackInfo &info) {
  float when = 0.0f;
  if (info.Length() > 0) {
    when = info[0].ToNumber();
  }
  if (std::isnan(when)) {
    Napi::TypeError::New(
        info.Env(), "Failed to execute 'start' on 'AudioScheduledSourceNode': "
                    "The provided double value is non-finite.\n")
        .ThrowAsJavaScriptException();
    return info.Env().Undefined();
  } else if (when < 0) {
    Napi_IH::TypeError::New(
        info.Env(),
        "Failed to execute 'start' on 'AudioScheduledSourceNode': The start "
        "time provided (%f) is less than the minimum bound (0).\n",
        when)
        .ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }

  std::static_pointer_cast<AudioScheduledSourceNode>(audio_node_ref_)
      ->Start(when);

  return info.Env().Undefined();
}

Napi::Value JSAudioScheduledSourceNode::Stop(const Napi::CallbackInfo &info) {
  float when = 0.0f;
  if (info.Length() > 0) {
    when = info[0].ToNumber();
  }

  if (std::isnan(when)) {
    Napi::TypeError::New(
        info.Env(), "Failed to execute 'stop' on 'AudioScheduledSourceNode': "
                    "The provided double value is non-finite.\n")
        .ThrowAsJavaScriptException();
    return info.Env().Undefined();
  } else if (when < 0) {
    Napi_IH::TypeError::New(
        info.Env(),
        "Failed to execute 'stop' on 'AudioScheduledSourceNode': The start "
        "time provided (%f) is less than the minimum bound (0).\n",
        when)
        .ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }

  std::static_pointer_cast<AudioScheduledSourceNode>(audio_node_ref_)
      ->Stop(when);

  return info.Env().Undefined();
}

} // namespace js_audio
