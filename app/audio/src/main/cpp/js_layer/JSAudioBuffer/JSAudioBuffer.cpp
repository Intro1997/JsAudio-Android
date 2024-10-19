#include "JSAudioBuffer.hpp"

namespace js_audio {
void JSAudioBuffer::Init(Napi::Env env, Napi::Object exports) {
  DefineClass<JSAudioBuffer>(
      env, "AudioBuffer",
      {InstanceAccessor<JSAudioBuffer, &JSAudioBuffer::getSampleRate>(
           "sampleRate"),
       InstanceAccessor<JSAudioBuffer, &JSAudioBuffer::getLength>("length"),
       InstanceAccessor<JSAudioBuffer, &JSAudioBuffer::getDuration>("duration"),
       InstanceAccessor<JSAudioBuffer, &JSAudioBuffer::getNumberOfChannels>(
           "numberOfChannels"),
       InstanceMethod<JSAudioBuffer, &JSAudioBuffer::copyToChannel>(
           "copyToChannel"),
       InstanceMethod<JSAudioBuffer, &JSAudioBuffer::getChannelData>(
           "getChannelData"),
       InstanceMethod<JSAudioBuffer, &JSAudioBuffer::copyFromChannel>(
           "copyFromChannel")},
      Napi_IH::ClassVisibility::kHideType);
}

JSAudioBuffer::JSAudioBuffer(const Napi_IH::IHCallbackInfo &info,
                             std::shared_ptr<AudioBuffer> audio_buffer_ptr)
    : Napi_IH::IHObjectWrap(info) {
  if (audio_buffer_ptr) {
    audio_buffer_ptr_ = audio_buffer_ptr;
  } else if (info.Length() < 3) {
    // TODO: maybe throw here
    LOGE("Create JSAudioBuffer failed! Need 3 parameters, but get %zu.\n",
         info.Length());
  } else if (info[0].IsNumber() && info[1].IsNumber() && info[2].IsNumber()) {
    uint32_t number_of_channels = info[0].As<Napi::Number>();
    uint32_t length = info[1].As<Napi::Number>();
    float sample_rate = info[2].As<Napi::Number>();
    audio_buffer_ptr_ =
        std::make_shared<AudioBuffer>(number_of_channels, length, sample_rate);
  } else {
    // TODO: maybe throw here
    LOGE("Create JSAudioBuffer failed! Need 3 number parameters.\n");
  }
}

Napi::Value JSAudioBuffer::getSampleRate(const Napi::CallbackInfo &info) {
  if (audio_buffer_ptr_) {
    return Napi::Number::From(info.Env(),
                              audio_buffer_ptr_->number_of_channel());
  }
  return info.Env().Undefined();
}
Napi::Value JSAudioBuffer::getLength(const Napi::CallbackInfo &info) {
  if (audio_buffer_ptr_) {
    return Napi::Number::From(info.Env(), audio_buffer_ptr_->length());
  }
  return info.Env().Undefined();
}
Napi::Value JSAudioBuffer::getDuration(const Napi::CallbackInfo &info) {
  if (audio_buffer_ptr_) {
    return Napi::Number::From(info.Env(), audio_buffer_ptr_->sample_rate());
  }
  return info.Env().Undefined();
}
Napi::Value JSAudioBuffer::getNumberOfChannels(const Napi::CallbackInfo &info) {
  if (audio_buffer_ptr_) {
    return Napi::Number::From(info.Env(), audio_buffer_ptr_->duration());
  }
  return info.Env().Undefined();
}
Napi::Value JSAudioBuffer::copyToChannel(const Napi::CallbackInfo &info) {
  if (info.Length() < 2) {
    // TODO: maybe throw here
    LOGE("Need 2 parameters, but get %zu\n", info.Length());
  } else if (info.Length() > 2 &&
             (!info[0].IsTypedArray() || !info[1].IsNumber() ||
              !info[2].IsNumber())) {
    // TODO: maybe throw here
    LOGE("The first parameter type must be Float32Array and the last 2 "
         "parameters type must be number!\n");
  } else if (info.Length() == 2 &&
             (!info[0].IsTypedArray() || !info[1].IsNumber())) {
    // TODO: maybe throw here
    LOGE("The first parameter type must be Float32Array and the second "
         "parameter type must be number!\n");
  } else {
    Napi::Float32Array js_source_array = info[0].As<Napi::Float32Array>();
    const uint32_t channel_number = info[1].As<Napi::Number>();
    // TODO: need channel_number validation here
    uint32_t start_in_channel = 0;
    if (info.Length() > 2) {
      start_in_channel = info[2].As<Napi::Number>();
    }

    float *source_array_ptr = js_source_array.Data();
    size_t source_array_size = js_source_array.ElementLength();
    audio_buffer_ptr_->CopyToChannel(source_array_ptr, source_array_size,
                                     channel_number, start_in_channel);
  }
  return info.Env().Undefined();
}

Napi::Value JSAudioBuffer::getChannelData(const Napi::CallbackInfo &info) {
  if (info.Length() < 1 || !info[0].IsNumber()) {
    LOGE("Need at least 1 parameter and this parameter must be number!\n");
    return info.Env().Undefined();
  }
  const uint32_t channel = info[0].As<Napi::Number>();
  // TODO: need channel_number validation here
  std::vector<float> channel_buffer =
      audio_buffer_ptr_->GetChannelData(channel);

  Napi::Float32Array js_ret_array =
      Napi::Float32Array::New(info.Env(), channel_buffer.size());
  float *ret_array_ptr = js_ret_array.Data();

  try {
    std::memcpy(ret_array_ptr, channel_buffer.data(),
                channel_buffer.size() * sizeof(channel_buffer[0]));
  } catch (std::exception e) {
    LOGE("Unknown std::memcpy exception: %s\n", e.what());
    return info.Env().Undefined();
  }

  return js_ret_array;
}

Napi::Value JSAudioBuffer::copyFromChannel(const Napi::CallbackInfo &info) {
  if (info.Length() < 2) {
    LOGE("Need 2 parameters, but get %zu\n", info.Length());
  } else if (info.Length() > 2 &&
             (!info[0].IsTypedArray() || !info[1].IsNumber() ||
              !info[2].IsNumber())) {
    LOGE("The first parameter type must be Float32Array and the last 2 "
         "parameters type must be number!\n");
  } else if (info.Length() == 2 &&
             (!info[0].IsTypedArray() || !info[1].IsNumber())) {
    LOGE("The first parameter type must be Float32Array and the second "
         "parameter type must be number!\n");
  } else {
    Napi::Float32Array js_dest_array = info[0].As<Napi::Float32Array>();

    float *dest_array = js_dest_array.Data();
    size_t js_dest_array_size = js_dest_array.ElementLength();
    uint32_t number_of_channel = info[1].As<Napi::Number>();
    uint32_t start_idx = 0;
    if (info.Length() > 2) {
      start_idx = info[2].As<Napi::Number>();
    }
    // TODO; need number_of_channel validation here
    // TODO; need start_idx validation here
    // TODO: need copy validation here

    audio_buffer_ptr_->CopyFromChannel(dest_array, js_dest_array_size,
                                       number_of_channel, start_idx);
  }
  return info.Env().Undefined();
}

}; // namespace js_audio