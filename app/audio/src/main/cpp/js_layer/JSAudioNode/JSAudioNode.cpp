#include "JSAudioNode.hpp"
#include "logger.hpp"

namespace js_audio {
Napi::FunctionReference JSAudioNode::js_audio_node_class_ref_;

JSAudioNode::JSAudioNode(const Napi_IH::IHCallbackInfo &info)
    : Napi_IH::IHObjectWrap(info) {
  if (info.Length() < 3) {
    LOGE(
        "Create JSAudioDestinationNode failed, need 3 arguments but get %zu.\n",
        info.Length());
    return;
  }
  Napi::Number number_of_inputs = info[0].As<Napi::Number>();
  Napi::Number number_of_outputs = info[1].As<Napi::Number>();
  Napi::Number channel_count = info[2].As<Napi::Number>();
  if (number_of_inputs.IsEmpty() || number_of_outputs.IsEmpty() ||
      channel_count.IsEmpty()) {
    LOGE("Create JSAudioDestinationNode failed, need 3 number variable!\n");
    return;
  }
  number_of_inputs_ = number_of_inputs.Uint32Value();
  number_of_outputs_ = number_of_outputs.Uint32Value();
  channel_count_ = channel_count.Uint32Value();
}

void JSAudioNode::Init(Napi::Env env, Napi::Object exports) {
  DefineClass<JSAudioNode>(
      env, "AudioNode",
      {InstanceAccessor<JSAudioNode, &JSAudioNode::GetNumberOfInputs>(
           "numberOfInputs"),
       InstanceAccessor<JSAudioNode, &JSAudioNode::GetNumberOfOutputs>(
           "numberOfOutputs"),
       InstanceAccessor<JSAudioNode, &JSAudioNode::GetChannelCount>(
           "channelCount")},
      false);
}

Napi::Value JSAudioNode::GetNumberOfInputs(const Napi::CallbackInfo &info) {
  return Napi::Value::From(info.Env(), number_of_inputs_);
}

Napi::Value JSAudioNode::GetNumberOfOutputs(const Napi::CallbackInfo &info) {
  return Napi::Value::From(info.Env(), number_of_outputs_);
}

Napi::Value JSAudioNode::GetChannelCount(const Napi::CallbackInfo &info) {
  return Napi::Value::From(info.Env(), channel_count_);
}

} // namespace js_audio