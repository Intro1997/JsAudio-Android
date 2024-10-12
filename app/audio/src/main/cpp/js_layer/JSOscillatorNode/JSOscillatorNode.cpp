#include <functional>

#include "JSAudioParam.hpp"
#include "JSOscillatorNode.hpp"

namespace js_audio {
JSOscillatorNode::JSOscillatorNode(
    const Napi_IH::IHCallbackInfo &info,
    std::shared_ptr<OscillatorNode> oscillator_node_ptr)
    : JSAudioScheduledSourceNode(info, oscillator_node_ptr),
      oscillator_node_ptr_(oscillator_node_ptr) {}

void JSOscillatorNode::Init(Napi::Env env, Napi::Object exports) {
  DefineClass<JSOscillatorNode, JSAudioScheduledSourceNode>(
      env, "OscillatorNode",
      {
          InstanceAccessor<JSOscillatorNode, &JSOscillatorNode::GetFrequency>(
              "frequency"),
          InstanceAccessor<JSOscillatorNode, &JSOscillatorNode::GetDetune>(
              "detune"),
          InstanceAccessor<JSOscillatorNode, &JSOscillatorNode::GetType,
                           &JSOscillatorNode::SetType>("type"),
      },
      false);
}

void JSOscillatorNode::SetType(const Napi::CallbackInfo &info,
                               const Napi::Value &value) {
  if (auto oscillator_node_ref = oscillator_node_ptr_.lock()) {
    if (!value.IsString()) {
      LOGE("Error! Type of oscillator node should be a string value!\n");
    }
    oscillator_node_ref->set_type(value.As<Napi::String>());
  } else {
    LOGE("Error! Inner oscillator node expired!\n");
  }
}

Napi::Value JSOscillatorNode::GetFrequency(const Napi::CallbackInfo &info) {
  if (auto oscillator_node_ref = oscillator_node_ptr_.lock()) {

    Napi::Object js_frequency =
        FindClass<JSAudioParam>().NewWithArgs<JSAudioParam>(
            {info.This()}, oscillator_node_ref->frequency());
    return js_frequency;
  }
  return info.Env().Undefined();
}

Napi::Value JSOscillatorNode::GetDetune(const Napi::CallbackInfo &info) {
  if (auto oscillator_node_ref = oscillator_node_ptr_.lock()) {
    Napi::Object js_detune =
        FindClass<JSAudioParam>().NewWithArgs<JSAudioParam>(
            {info.This()}, oscillator_node_ref->detune());
    return js_detune;
  }
  return info.Env().Undefined();
}

Napi::Value JSOscillatorNode::GetType(const Napi::CallbackInfo &info) {
  if (auto oscillator_node_ref = oscillator_node_ptr_.lock()) {
    Napi::String type_value =
        Napi::String::New(info.Env(), oscillator_node_ref->type());
    return type_value;
  }
  return info.Env().Undefined();
}

} // namespace js_audio
