#include "JSAudioDestinationNode.hpp"
#include "logger.hpp"
#include <memory>

namespace js_audio {

JSAudioDestinationNode::JSAudioDestinationNode(
    const Napi_IH::IHCallbackInfo &info,
    std::shared_ptr<AudioDestinationNode> audio_destination_node_ptr)
    : JSAudioNode(info, audio_destination_node_ptr),
      audio_destination_node_ref_(
          std::static_pointer_cast<AudioDestinationNode>(
              JSAudioNode::audio_node_ptr_)) {}

void JSAudioDestinationNode::Init(Napi::Env env, Napi::Object exports) {
  DefineClass<JSAudioDestinationNode, JSAudioNode>(
      env, "AudioDestinationNode", {},
      Napi_IH::ClassVisibility::kHideConstructor);
}

}; // namespace js_audio