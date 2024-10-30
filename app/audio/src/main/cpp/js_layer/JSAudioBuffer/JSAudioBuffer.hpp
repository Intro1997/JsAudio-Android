#pragma once
#include "AudioBuffer.hpp"

#include <napi_ih/napi_ih.hpp>
#include <vector>

namespace js_audio {
class JSAudioBuffer : public Napi_IH::IHObjectWrap {
public:
  static void Init(Napi::Env env, Napi::Object exports);

  JSAudioBuffer(const Napi_IH::IHCallbackInfo &info,
                std::shared_ptr<AudioBuffer> audio_buffer_ptr = nullptr);

  Napi::Value getSampleRate(const Napi::CallbackInfo &info);
  Napi::Value getLength(const Napi::CallbackInfo &info);
  Napi::Value getDuration(const Napi::CallbackInfo &info);
  Napi::Value getNumberOfChannels(const Napi::CallbackInfo &info);

  Napi::Value copyToChannel(const Napi::CallbackInfo &info);
  Napi::Value getChannelData(const Napi::CallbackInfo &info);
  Napi::Value copyFromChannel(const Napi::CallbackInfo &info);

private:
  std::vector<Napi::ObjectReference> js_channels_ref_;
  std::shared_ptr<AudioBuffer> audio_buffer_ptr_;
};
} // namespace js_audio
