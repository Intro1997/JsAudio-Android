#include "JSAudioDestinationNode.hpp"
#include "logger.hpp"
#include <memory>

namespace js_audio {

JSAudioDestinationNode::JSAudioDestinationNode(
    const Napi_IH::IHCallbackInfo &info,
    std::shared_ptr<AudioDestinationNode> audio_destination_node_ptr)
    : JSAudioNode(info, audio_destination_node_ptr),
      audio_destination_node_ptr_(
          std::static_pointer_cast<AudioDestinationNode>(
              JSAudioNode::audio_node_ptr_)) {}

void JSAudioDestinationNode::Init(Napi::Env env, Napi::Object exports) {
  DefineClass<JSAudioDestinationNode, JSAudioNode>(
      env, "AudioDestinationNode",
      {InstanceAccessor<JSAudioDestinationNode,
                      &JSAudioDestinationNode::getMaxChannelCount>(
          "maxChannelCount")},
      Napi_IH::ClassVisibility::kHideConstructor);
}

Napi::Value
JSAudioDestinationNode::getMaxChannelCount(const Napi::CallbackInfo &info) {
  if (auto audio_destination_node_ref = audio_destination_node_ptr_.lock()) {
    return Napi::Value::From(info.Env(),
                             audio_destination_node_ref->max_channel_count());
  }
  return info.Env().Undefined();
}

}; // namespace js_audio