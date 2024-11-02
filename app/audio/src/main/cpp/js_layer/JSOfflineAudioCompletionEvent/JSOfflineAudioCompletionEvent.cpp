#include "JSOfflineAudioCompletionEvent.hpp"
#include "JSAudioBuffer.hpp"

namespace js_audio {
void JSOfflineAudioCompletionEvent::Init(Napi::Env env, Napi::Object exports) {
  DefineClass<JSOfflineAudioCompletionEvent>(
      env, "OfflineAudioCompletionEvent",
      {InstanceAccessor<JSOfflineAudioCompletionEvent,
                        &JSOfflineAudioCompletionEvent::getAudioBuffer>(
          "renderedBuffer")},
      Napi_IH::ClassVisibility::kHideConstructor);
}

JSOfflineAudioCompletionEvent::JSOfflineAudioCompletionEvent(
    const Napi_IH::IHCallbackInfo &info)
    : Napi_IH::IHObjectWrap(info) {
  if (info.Length() < 1 && !info[0].IsObject()) {
    LOGE("Create JSOfflineAudioCompletionEvent failed! Cannot create "
         "JSOfflineAudioCompletionEvent without AudioBuffer object.\n");
    return;
  }
  napi_ret_audio_buffer_ref_ = Napi::Persistent(info[0].As<Napi::Object>());
}

Napi::Value
JSOfflineAudioCompletionEvent::getAudioBuffer(const Napi::CallbackInfo &info) {
  return napi_ret_audio_buffer_ref_.Value();
}
} // namespace js_audio
