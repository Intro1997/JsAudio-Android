#include "JSBaseAudioContext.hpp"
#include "JSAudioDestinationNode.hpp"
namespace js_audio {
JSBaseAudioContext::JSBaseAudioContext(const Napi_IH::IHCallbackInfo &info)
    : Napi_IH::IHObjectWrap(info) {
  Napi::Env env = info.Env();
  Napi::Object js_destination_node =
      JSAudioDestinationNode::FindClass<JSAudioDestinationNode>().New({
          Napi::Number::From(env,
                             1), // TODO: Use default value of specification
          Napi::Number::From(env,
                             0), // TODO: Use default value of specification
          Napi::Number::From(env,
                             2), // TODO: Use default value of specification
      });
  js_destination_node_ref_ = Napi::Persistent(js_destination_node);
  sample_rate_ = 44100; // TODO: get from cpp side
  current_time_ = 0.0;  // TODO: get from cpp side
}

void JSBaseAudioContext::Init(Napi::Env env, Napi::Object exports) {
  DefineClass<JSBaseAudioContext>(
      env, "BaseAudioContext",
      {
          InstanceAccessor<JSBaseAudioContext,
                           &JSBaseAudioContext::GetDestinationNode>(
              "destination"),
          InstanceAccessor<JSBaseAudioContext,
                           &JSBaseAudioContext::GetSampleRate>("sampleRate"),
          InstanceAccessor<JSBaseAudioContext,
                           &JSBaseAudioContext::GetCurrentTime>("currentTime"),
      },
      false);
}

Napi::Value
JSBaseAudioContext::GetDestinationNode(const Napi::CallbackInfo &info) {
  return js_destination_node_ref_.Value();
}

Napi::Value JSBaseAudioContext::GetSampleRate(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  return Napi::Number::From(env, sample_rate_);
}

Napi::Value JSBaseAudioContext::GetCurrentTime(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  return Napi::Number::From(env, current_time_);
}
} // namespace js_audio