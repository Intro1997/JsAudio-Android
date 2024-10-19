#pragma once
#include "JSBaseAudioContext.hpp"
#include <napi/napi.h>
namespace js_audio {
class JSAudioContext : public JSBaseAudioContext {
public:
  static void Init(Napi::Env napi_env, Napi::Object exports);

public:
  JSAudioContext(const Napi_IH::IHCallbackInfo &info);

  Napi::Value resume(const Napi::CallbackInfo &info);
  Napi::Value suspend(const Napi::CallbackInfo &info);
  Napi::Value close(const Napi::CallbackInfo &info);
};
} // namespace js_audio
