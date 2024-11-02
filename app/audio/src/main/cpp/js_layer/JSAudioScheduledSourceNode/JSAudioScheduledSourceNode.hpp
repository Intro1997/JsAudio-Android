#pragma once
#include "AudioScheduledSourceNode.hpp"
#include "JSAudioNode.hpp"
namespace js_audio {
class JSAudioScheduledSourceNode : public JSAudioNode {
public:
  JSAudioScheduledSourceNode(const Napi_IH::IHCallbackInfo &info,
                             std::shared_ptr<AudioScheduledSourceNode>
                                 audio_scheduled_source_node_ref = nullptr);

  static void Init(Napi::Env env, Napi::Object exports);

  Napi::Value Start(const Napi::CallbackInfo &info);
  Napi::Value Stop(const Napi::CallbackInfo &info);
};
} // namespace js_audio
