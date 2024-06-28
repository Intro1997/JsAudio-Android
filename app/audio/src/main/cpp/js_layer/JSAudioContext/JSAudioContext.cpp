#include "JSAudioContext.hpp"
#include "JSAudioDestinationNode.hpp"
namespace js_audio {

Napi::Value JSAudioContext::Init(Napi::Env napi_env, Napi::Object exports) {
  Napi::Function js_audio_context_constructor = DefineClass(
      napi_env, "AudioContext",
      {InstanceAccessor<&JSAudioContext::GetSampleRate>("sampleRate"),
       InstanceAccessor<&JSAudioContext::GetDestinationNode>("destination")});

  Napi::FunctionReference *constructor_ref = new Napi::FunctionReference();
  *constructor_ref = Napi::Persistent(js_audio_context_constructor);
  exports.Set("AudioContext", js_audio_context_constructor);
  napi_env.SetInstanceData<Napi::FunctionReference>(constructor_ref);
  return exports;
}

JSAudioContext::JSAudioContext(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<JSAudioContext>(info), sample_rate_(48000.0f) {
  Napi::Env env = info.Env();
  Napi::Object js_destination_node = JSAudioDestinationNode::GetClass(env).New({
      Napi::Number::From(env, 1),
      Napi::Number::From(env, 0),
      Napi::Number::From(env, 2),
  });
  js_destination_node_ref_ = Napi::Persistent(js_destination_node);
}

Napi::Value JSAudioContext::GetSampleRate(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  return Napi::Number::New(env, sample_rate_);
}

Napi::Value JSAudioContext::GetDestinationNode(const Napi::CallbackInfo &info) {
  return js_destination_node_ref_.Value();
}

}; // namespace js_audio
