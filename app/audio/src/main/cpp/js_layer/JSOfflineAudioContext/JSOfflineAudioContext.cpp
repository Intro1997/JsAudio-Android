#include "JSOfflineAudioContext.hpp"
#include "AudioBuffer.hpp"
#include "JSAudioBuffer.hpp"
#include "JSOfflineAudioCompletionEvent.hpp"
#include "OfflineAudioContext.hpp"
#include "logger.hpp"
#include <napi/napi.h>

struct RenderedAudioBufferWrapper {
  std::shared_ptr<js_audio::AudioBuffer> rendered_audio_buffer_ref;
};
using OfflineAudioContext = js_audio::OfflineAudioContext;
using ContextType = Napi::Promise::Deferred;
using DataType = RenderedAudioBufferWrapper;

static void CallJs(Napi::Env env, Napi::Function js_func,
                   ContextType *js_promise_ptr, DataType *value);

using TSFN = Napi::TypedThreadSafeFunction<ContextType, DataType, CallJs>;

static std::shared_ptr<OfflineAudioContext>
GetOfflineAudioContextRef(const Napi_IH::IHCallbackInfo &info);

namespace js_audio {
void JSOfflineAudioContext::Init(Napi::Env env, Napi::Object exports) {
  DefineClass<JSOfflineAudioContext, JSBaseAudioContext>(
      env, "OfflineAudioContext",
      {InstanceMethod<JSOfflineAudioContext,
                      &JSOfflineAudioContext::startRendering>("startRendering"),
       InstanceAccessor<JSOfflineAudioContext,
                        &JSOfflineAudioContext::getLength>("length")});
}

JSOfflineAudioContext::JSOfflineAudioContext(
    const Napi_IH::IHCallbackInfo &info)
    : JSBaseAudioContext(info, GetOfflineAudioContextRef(info)),
      oncomplete_promise_(Napi::Promise::Deferred::New(info.Env())) {}

Napi::Value
JSOfflineAudioContext::startRendering(const Napi::CallbackInfo &info) {

  ContextType *js_promise_ptr = new Napi::Promise::Deferred(info.Env());
  TSFN tsfn;

  tsfn = TSFN::New(info.Env(), "StartRendering TSFN", 0, 1, js_promise_ptr);

  std::function<void(std::shared_ptr<AudioBuffer>)> cb_ref =
      [=](std::shared_ptr<AudioBuffer> rendered_audio_buffer_ref) {
        RenderedAudioBufferWrapper *wrapper = new RenderedAudioBufferWrapper();
        wrapper->rendered_audio_buffer_ref = rendered_audio_buffer_ref;
        napi_status status = tsfn.NonBlockingCall(wrapper);
        if (status != napi_ok) {
          LOGE("NonBlockingCall failed: error code is %d\n", status);
        }
        tsfn.Release();
      };

  bool is_start =
      std::static_pointer_cast<OfflineAudioContext>(base_audio_context_ref_)
          ->StartRendering(cb_ref, base_audio_context_ref_);
  if (!is_start) {
    LOGE("InvalidStateError: Failed to execute 'startRendering' on "
         "'OfflineAudioContext': cannot startRendering when an "
         "OfflineAudioContext is running\n");
  }
  return js_promise_ptr->Promise();
}

Napi::Value JSOfflineAudioContext::getLength(const Napi::CallbackInfo &info) {
  uint32_t length =
      std::static_pointer_cast<OfflineAudioContext>(base_audio_context_ref_)
          ->length();
  return Napi::Number::From(info.Env(), length);
}

} // namespace js_audio

static std::shared_ptr<OfflineAudioContext>
GetOfflineAudioContextRef(const Napi_IH::IHCallbackInfo &info) {
  uint32_t number_of_channels = 1;
  uint32_t length = 0;
  float sample_rate = 0.0f;

  if (info.Length() < 1) {

    throw Napi::TypeError::New(info.Env(),
                               "Failed to construct 'OfflineAudioContext': 1 "
                               "argument required, but only 0 present.\n");
  }
  if (info.Length() == 1 && !info[0].IsObject()) {
    throw Napi::TypeError::New(
        info.Env(), "Failed to construct 'OfflineAudioContext': The provided "
                    "value is not of type 'OfflineAudioContextOptions'");
  }

  if (info[0].IsObject()) {
    Napi::Object options = info[0].As<Napi::Object>();
    if (!options.Has("length")) {
      throw Napi::TypeError::New(
          info.Env(),
          "Failed to construct 'OfflineAudioContext': Failed to read the "
          "'length' property from 'OfflineAudioContextOptions': Required "
          "member is undefined.");
    }
    if (!options.Has("sampleRate")) {
      throw Napi::TypeError::New(
          info.Env(),
          "Failed to construct 'OfflineAudioContext': Failed to read the "
          "'sampleRate' property from 'OfflineAudioContextOptions': Required "
          "member is undefined.");
    }
    length = options.Get("length").As<Napi::Number>().operator unsigned int();
    sample_rate = options.Get("sampleRate").As<Napi::Number>().operator float();
    if (options.Has("numberOfChannels")) {
      number_of_channels = options.Get("numberOfChannels")
                               .As<Napi::Number>()
                               .operator unsigned int();
    }

  } else {
    if (info.Length() < 3) {
      throw Napi::TypeError::New(
          info.Env(), "Failed to construct 'OfflineAudioContext': Need 3 "
                      "parameters but the provided values are 2");
    }

    if (!info[0].IsNumber() || !info[1].IsNumber() || !info[2].IsNumber()) {
      throw Napi::TypeError::New(
          info.Env(),
          "Failed to construct 'OfflineAudioContext': Need 3 number type "
          "parameters, but has parameter in other value type.");
    }
    number_of_channels = info[0].As<Napi::Number>().operator float();
    length = info[1].As<Napi::Number>().operator unsigned int();
    sample_rate = info[2].As<Napi::Number>().operator float();
  }

  if (number_of_channels < 1 || number_of_channels > 32) {
    auto ret = Napi_IH::RangeError::New(
        info.Env(),
        "Failed to construct 'OfflineAudioContext': The number of channels "
        "provided (%u) is outside the range [1, 32].",
        number_of_channels);
    throw ret;
  }
  if (length < 1) {
    throw Napi_IH::RangeError::New(
        info.Env(),
        "Failed to construct 'OfflineAudioContext': The number of frames "
        "provided (%u) is less than the minimum bound (1).",
        length);
  }
  if (sample_rate < 3000 || sample_rate > 768000) {
    throw Napi_IH::RangeError::New(info.Env(),
                                   "Failed to construct 'OfflineAudioContext': "
                                   "The sampleRate provided (%f) "
                                   "is outside the range [3000, 768000].",
                                   sample_rate);
  }

  return std::make_shared<OfflineAudioContext>(number_of_channels, length,
                                               sample_rate);
}

static void CallJs(Napi::Env env, Napi::Function js_func,
                   ContextType *js_promise_ptr, DataType *value) {
  Napi::Object js_rendered_audio_buffer =
      js_audio::JSAudioBuffer::FindClass<js_audio::JSAudioBuffer>()
          .NewWithArgs<js_audio::JSAudioBuffer>(
              {}, value->rendered_audio_buffer_ref);

  js_promise_ptr->Resolve(js_rendered_audio_buffer);

  delete value;
  delete js_promise_ptr;
}
