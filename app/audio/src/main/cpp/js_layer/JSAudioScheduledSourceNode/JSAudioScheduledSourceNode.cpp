#include "JSAudioScheduledSourceNode.hpp"

namespace js_audio {
JSAudioScheduledSourceNode::JSAudioScheduledSourceNode(
    const Napi_IH::IHCallbackInfo &info,
    std::shared_ptr<AudioScheduledSourceNode> audio_scheduled_source_node_ptr)
    : JSAudioNode(info, audio_scheduled_source_node_ptr) {}

void JSAudioScheduledSourceNode::Init(Napi::Env env, Napi::Object exports) {
  DefineClass<JSAudioScheduledSourceNode, JSAudioNode>(
      env, "AudioScheduledSourceNode", {}, true, true);
}
} // namespace js_audio
