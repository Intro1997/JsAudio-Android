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
                      &JSAudioScheduledSourceNode::start>("start"),
       InstanceMethod<JSAudioScheduledSourceNode,
                      &JSAudioScheduledSourceNode::stop>("stop")},
      Napi_IH::ClassVisibility::kHideConstructor);
}

Napi::Value JSAudioScheduledSourceNode::start(const Napi::CallbackInfo &info) {
  double when = 0.0f;
  if (info.Length() > 0) {
    when = info[0].ToNumber();
    Napi::Error napi_error;
    if (!IsWhenValid(when, "AudioScheduledSourceNode", "start", info.Env(),
                     napi_error)) {
      throw napi_error;
    }
  }

  if (audio_node_ref_) {
    std::static_pointer_cast<AudioScheduledSourceNode>(audio_node_ref_)
        ->Start(when);
  }
  return info.Env().Undefined();
}

Napi::Value JSAudioScheduledSourceNode::stop(const Napi::CallbackInfo &info) {
  if (audio_node_ref_) {
    if (!std::static_pointer_cast<AudioScheduledSourceNode>(audio_node_ref_)
             ->has_started()) {
      throw Napi::Error::New(
          info.Env(), "Failed to execute 'stop' on 'AudioScheduledSourceNode': "
                      "cannot call stop without calling start first.\n");
    }
  } else {
    LOGE("Invalid inner audio node reference!\n");
    return info.Env().Undefined();
  }

  double when = 0.0f;
  if (info.Length() > 0) {
    when = info[0].ToNumber();
    Napi::Error napi_error;
    if (!IsWhenValid(when, "AudioScheduledSourceNode", "stop", info.Env(),
                     napi_error)) {
      throw napi_error;
    }
  }

  std::static_pointer_cast<AudioScheduledSourceNode>(audio_node_ref_)
      ->Stop(when);

  return info.Env().Undefined();
}

bool JSAudioScheduledSourceNode::IsWhenValid(const double &when,
                                             const char *type_name,
                                             const char *func_name,
                                             Napi::Env env,
                                             Napi::Error &napi_error) {
  if (std::isnan(when) || std::isinf(when)) {
    napi_error =
        Napi_IH::TypeError::New(env,
                                "Failed to execute '%s' on '%s': The "
                                "provided double value is non-finite.\n",
                                func_name, type_name);
    return false;
  } else if (when < 0) {
    napi_error = Napi_IH::RangeError::New(
        env,
        "Failed to execute '%s' on '%s': The start time provided (%f) is "
        "less than the minimum bound (0).\n",
        func_name, type_name, when);
    return false;
  }
  return true;
}
} // namespace js_audio
