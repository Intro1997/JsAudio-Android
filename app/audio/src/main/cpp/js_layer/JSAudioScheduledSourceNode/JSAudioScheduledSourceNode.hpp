#pragma once
#include "AudioScheduledSourceNode.hpp"
#include "JSAudioNode.hpp"
namespace js_audio {
class JSAudioScheduledSourceNode : public JSAudioNode {
public:
  JSAudioScheduledSourceNode(const Napi_IH::IHCallbackInfo &info,
                             std::shared_ptr<AudioScheduledSourceNode>
                                 audio_scheduled_source_node_ptr = nullptr);

  static void Init(Napi::Env env, Napi::Object exports);
};
} // namespace js_audio
