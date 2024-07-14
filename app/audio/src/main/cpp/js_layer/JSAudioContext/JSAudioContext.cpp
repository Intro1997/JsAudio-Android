#include "JSAudioContext.hpp"
#include "AudioContext.hpp"
#include "JSAudioDestinationNode.hpp"

#include <napi_ih/napi_ih.hpp>

namespace js_audio {

void JSAudioContext::Init(Napi::Env env, Napi::Object exports) {
  DefineClass<JSAudioContext, JSBaseAudioContext>(
      env, "AudioContext",
      {
          InstanceMethod<JSAudioContext, &JSAudioContext::Resume>("resume"),
          InstanceMethod<JSAudioContext, &JSAudioContext::Suspend>("suspend"),
          InstanceMethod<JSAudioContext, &JSAudioContext::Close>("close"),
      });
}

JSAudioContext::JSAudioContext(const Napi_IH::IHCallbackInfo &info)
    : JSBaseAudioContext(info, std::make_shared<AudioContext>()) {}

Napi::Value JSAudioContext::Resume(const Napi::CallbackInfo &info) {
  // TODO: make it return promise object
  LOGD("Get in JSAudioContext::Resume()\n");
  return {};
}

Napi::Value JSAudioContext::Suspend(const Napi::CallbackInfo &info) {
  // TODO: make it return promise object
  LOGD("Get in JSAudioContext::Suspend()\n");
  return {};
}

Napi::Value JSAudioContext::Close(const Napi::CallbackInfo &info) {
  // TODO: make it return promise object
  LOGD("Get in JSAudioContext::Close()\n");
  return {};
}
}; // namespace js_audio
