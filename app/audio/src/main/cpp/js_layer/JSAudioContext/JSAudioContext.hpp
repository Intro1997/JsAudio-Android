#pragma once
#include "JSBaseAudioContext.hpp"
#include <napi/napi.h>
namespace js_audio {
class JSAudioContext : public JSBaseAudioContext {
public:
  static void Init(Napi::Env napi_env, Napi::Object exports);

public:
  JSAudioContext(const Napi_IH::IHCallbackInfo &info);

  Napi::Value Resume(const Napi::CallbackInfo &info);
  Napi::Value Suspend(const Napi::CallbackInfo &info);
  Napi::Value Close(const Napi::CallbackInfo &info);
};
} // namespace js_audio
