#include "JSAudioDestinationNode.hpp"
#include "logger.hpp"

namespace js_audio {
Napi::FunctionReference JSAudioDestinationNode::js_destination_node_class_ref_;

JSAudioDestinationNode::JSAudioDestinationNode(const Napi::CallbackInfo &info)
    : JSAudioNode(info) {}

Napi::Function JSAudioDestinationNode::GetClass(Napi::Env env) {
  if (js_destination_node_class_ref_.IsEmpty()) {
    Napi::Function js_destination_node_class =
        DefineClass(env, "AudioDestinationNode", {});
    js_destination_node_class_ref_ =
        Napi::Persistent(js_destination_node_class);
  }
  return js_destination_node_class_ref_.Value();
}

void JSAudioDestinationNode::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function audio_destination_node_class =
      JSAudioDestinationNode::GetClass(env);
  Napi::Function audio_node_class = JSAudioNode::GetClass(env);

  Napi::Function setProto = env.Global()
                                .Get("Object")
                                .ToObject()
                                .Get("setPrototypeOf")
                                .As<Napi::Function>();
  setProto.Call({audio_destination_node_class, audio_node_class});
  setProto.Call({audio_destination_node_class.Get("prototype"),
                 audio_node_class.Get("prototype")});
}
}; // namespace js_audio