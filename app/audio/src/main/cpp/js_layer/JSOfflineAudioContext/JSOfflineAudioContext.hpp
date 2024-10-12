#pragma once
#include "JSBaseAudioContext.hpp"
#include <napi/napi.h>
namespace js_audio {
class JSOfflineAudioContext : public JSBaseAudioContext {
public:
  static void Init(Napi::Env env, Napi::Object exports);

public:
  JSOfflineAudioContext(const Napi_IH::IHCallbackInfo &info);

  // Napi::Value Resume(const Napi::CallbackInfo &info);
  // Napi::Value Suspend(const Napi::CallbackInfo &info);
  Napi::Value StartRendering(const Napi::CallbackInfo &info);
  
  Napi::Value GetLength(const Napi::CallbackInfo &info);

private:
  Napi::Promise::Deferred oncomplete_promise_;
};
} // namespace js_audio
