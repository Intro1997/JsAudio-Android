#include "JSAudioDestinationNode.hpp"
#include "logger.hpp"

namespace js_audio {

JSAudioDestinationNode::JSAudioDestinationNode(
    const Napi_IH::IHCallbackInfo &info)
    : JSAudioNode(info), destination_specified_(123123123) {}

void JSAudioDestinationNode::Init(Napi::Env env, Napi::Object exports) {
  DefineClass<JSAudioDestinationNode, JSAudioNode>(
      env, "AudioDestinationNode",
      {
          InstanceAccessor<JSAudioDestinationNode,
                           &JSAudioDestinationNode::GetDestinationSpecified,
                           &JSAudioDestinationNode::SetDestinationSpecified>(
              "destinationSpecified"),
      });
}

Napi::Value JSAudioDestinationNode::GetDestinationSpecified(
    const Napi::CallbackInfo &info) {
  return Napi::Number::From(info.Env(), destination_specified_);
}

void JSAudioDestinationNode::SetDestinationSpecified(
    const Napi::CallbackInfo &info, const Napi::Value &value) {
  if (value.IsNumber()) {
    destination_specified_ = value.As<Napi::Number>().Uint32Value();
  } else {
    LOGE(
        "Value is not number type! Cannot set value to destinationSpecified\n");
  }
}

}; // namespace js_audio