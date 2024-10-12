#pragma once
#include "AudioBuffer.hpp"
#include <napi_ih/napi_ih.hpp>

namespace js_audio {
class JSAudioBuffer : public Napi_IH::IHObjectWrap {
public:
  static void Init(Napi::Env env, Napi::Object exports);

  JSAudioBuffer(const Napi_IH::IHCallbackInfo &info,
                std::shared_ptr<AudioBuffer> audio_buffer_ptr = nullptr);

  Napi::Value GetSampleRate(const Napi::CallbackInfo &info);
  Napi::Value GetLength(const Napi::CallbackInfo &info);
  Napi::Value GetDuration(const Napi::CallbackInfo &info);
  Napi::Value GetNumberOfChannels(const Napi::CallbackInfo &info);

  Napi::Value copyToChannel(const Napi::CallbackInfo &info);
  Napi::Value getChannelData(const Napi::CallbackInfo &info);
  Napi::Value copyFromChannel(const Napi::CallbackInfo &info);

private:
  std::shared_ptr<AudioBuffer> audio_buffer_ptr_;
};
} // namespace js_audio
