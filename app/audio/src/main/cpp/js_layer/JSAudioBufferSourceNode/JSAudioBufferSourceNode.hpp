#pragma once
#include "AudioBufferSourceNode.hpp"
#include "JSAudioScheduledSourceNode.hpp"
namespace js_audio {
class JSAudioBufferSourceNode : public JSAudioScheduledSourceNode {
public:
  static void Init(Napi::Env env, Napi::Object exports);

  JSAudioBufferSourceNode(const Napi_IH::IHCallbackInfo &info,
                          std::shared_ptr<AudioBufferSourceNode>
                              audio_buffer_source_node_ref = nullptr);
  // buffer: AudioBuffer | null;
  Napi::Value getBuffer(const Napi::CallbackInfo &info);
  // only can be set once
  void setBuffer(const Napi::CallbackInfo &info, const Napi::Value &value);

  // readonly detune: AudioParam;
  Napi::Value getDetune(const Napi::CallbackInfo &info);

  // loop: boolean;
  Napi::Value getLoop(const Napi::CallbackInfo &info);
  void setLoop(const Napi::CallbackInfo &info, const Napi::Value &value);

  // loopEnd: number;
  Napi::Value getLoopEnd(const Napi::CallbackInfo &info);
  void setLoopEnd(const Napi::CallbackInfo &info, const Napi::Value &value);

  // loopStart: number;
  Napi::Value getLoopStart(const Napi::CallbackInfo &info);
  void setLoopStart(const Napi::CallbackInfo &info, const Napi::Value &value);

  // readonly playbackRate: AudioParam;
  Napi::Value getPlaybackRate(const Napi::CallbackInfo &info);

  // start(when?: number, offset?: number, duration?: number): void;
  Napi::Value start(const Napi::CallbackInfo &info) override;

private:
  bool has_set_buffer_;
  Napi::ObjectReference napi_audio_buffer_ref_;
  std::weak_ptr<AudioBufferSourceNode> audio_buffer_source_node_ptr_;
};
} // namespace js_audio
