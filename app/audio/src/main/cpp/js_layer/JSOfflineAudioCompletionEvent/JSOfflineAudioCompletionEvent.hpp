#pragma once
#include <napi_ih/napi_ih.hpp>
namespace js_audio {
class JSOfflineAudioCompletionEvent : public Napi_IH::IHObjectWrap {
public:
  static void Init(Napi::Env env, Napi::Object exports);
  Napi::Value getAudioBuffer(const Napi::CallbackInfo &info);

  JSOfflineAudioCompletionEvent(const Napi_IH::IHCallbackInfo &info);

private:
  Napi::ObjectReference napi_ret_audio_buffer_ref_;
};
} // namespace js_audio
