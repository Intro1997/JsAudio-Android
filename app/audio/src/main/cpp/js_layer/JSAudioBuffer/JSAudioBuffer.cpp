#include "JSAudioBuffer.hpp"
#include <cmath>

using AudioBuffer = js_audio::AudioBuffer;
static std::shared_ptr<AudioBuffer>
GetAudioBufferRef(const Napi_IH::IHCallbackInfo &info) {
  if (info.Length() < 1 || !info[0].IsObject()) {
    throw Napi::TypeError::New(info.Env(),
                               "Failed to construct 'AudioBuffer': 1 argument "
                               "required, but only 0 present.\n");
  }

  Napi::Object options = info[0].As<Napi::Object>();
  {
    // after check, we do not need required_member anymore
    // so we wrap it with code block
    std::vector<std::string> required_member = {"length", "sampleRate"};
    for (const auto &member : required_member) {
      if (!options.Has(member)) {
        throw Napi_IH::TypeError::New(
            info.Env(),
            "Failed to construct 'AudioBuffer': Failed to read the '%s' "
            "property "
            "from 'AudioBufferOptions': Required member is undefined.\n",
            member.c_str());
      }
    }
  }
  uint32_t number_of_channels = 1;
  const uint32_t length = options.Get("length").ToNumber();
  const float sample_rate = options.Get("sampleRate").ToNumber();
  // TODO: move all check to a util header
  if (options.Has("numberOfChannels")) {
    number_of_channels = options.Get("numberOfChannels").ToNumber();
    if (number_of_channels < 1 || number_of_channels > 32) {
      throw Napi_IH::RangeError::New(
          info.Env(),
          "Failed to construct 'AudioBuffer': The number of channels provided "
          "(%u) is outside the range [1, 32].\n",
          number_of_channels);
    }
  }
  if (length < 1) {
    throw Napi_IH::RangeError::New(
        info.Env(),
        "Uncaught NotSupportedError: Failed to construct 'AudioBuffer': The "
        "number of frames provided (%u) is less than or equal to the minimum "
        "bound (0).\n",
        length);
  }
  if (std::isnan(sample_rate)) {
    throw Napi::TypeError::New(
        info.Env(), "Failed to construct 'AudioBuffer': Failed to read the "
                    "'sampleRate' property from 'AudioBufferOptions': The "
                    "provided float value is non-finite.\n");
  } else if (sample_rate < 3000 || sample_rate > 768000) {
    throw Napi_IH::RangeError::New(
        info.Env(),
        "Failed to construct 'AudioBuffer': The sample rate provided (%f) is "
        "outside the range [3000, 768000].\n",
        sample_rate);
  }
  return std::make_shared<AudioBuffer>(number_of_channels, length, sample_rate);
}

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
           "copyFromChannel")});
}

JSAudioBuffer::JSAudioBuffer(const Napi_IH::IHCallbackInfo &info,
                             std::shared_ptr<AudioBuffer> audio_buffer_ptr)
    : Napi_IH::IHObjectWrap(info) {
  if (audio_buffer_ptr) {
    audio_buffer_ptr_ = audio_buffer_ptr;
  } else {
    audio_buffer_ptr_ = GetAudioBufferRef(info);
  }

  if (!audio_buffer_ptr_) {
    LOGE("Error! Innner auido buffer pointer reference is invalid!\n");
    throw Napi::Error::New(info.Env(), "Inner error!\n");
  }

  for (int i = 0; i < audio_buffer_ptr_->number_of_channel(); i++) {
    float *data_pointer = audio_buffer_ptr_->audio_channel_buffers_[i].data();
    size_t data_size = audio_buffer_ptr_->audio_channel_buffers_[i].size();

    Napi::ArrayBuffer channel_array_buffer = Napi::ArrayBuffer::New(
        info.Env(), data_pointer, sizeof(float) * data_size);

    js_channels_ref_.push_back(Napi::Persistent(
        Napi::Float32Array::New(info.Env(), data_size, channel_array_buffer, 0)
            .ToObject()));
  }
}

Napi::Value JSAudioBuffer::getSampleRate(const Napi::CallbackInfo &info) {
  if (audio_buffer_ptr_) {
    return Napi::Number::From(info.Env(), audio_buffer_ptr_->sample_rate());
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
    return Napi::Number::From(info.Env(), audio_buffer_ptr_->duration());
  }
  return info.Env().Undefined();
}
Napi::Value JSAudioBuffer::getNumberOfChannels(const Napi::CallbackInfo &info) {
  if (audio_buffer_ptr_) {
    return Napi::Number::From(info.Env(),
                              audio_buffer_ptr_->number_of_channel());
  }
  return info.Env().Undefined();
}
Napi::Value JSAudioBuffer::copyToChannel(const Napi::CallbackInfo &info) {
  if (info.Length() < 2) {
    throw Napi_IH::TypeError::New(
        info.Env(),
        "TypeError: Failed to execute 'copyToChannel' on 'AudioBuffer': 2 "
        "arguments required, but only %zu present.\n",
        info.Length());
  }

  if (!info[0].IsTypedArray()) {
    throw Napi::TypeError::New(
        info.Env(),
        "Failed to execute 'copyToChannel' on 'AudioBuffer': parameter 1 is "
        "not of type 'Float32Array'.\n");
  } else {
    std::string constructor_name;
    Napi_IH::GetObjectConstrcutorName(info[0].As<Napi::Object>(),
                                      constructor_name);
    if (constructor_name != "Float32Array") {
      throw Napi::TypeError::New(info.Env(),
                                 "Failed to execute 'copyToChannel' on "
                                 "'AudioBuffer': parameter 1 is "
                                 "not of type 'Float32Array'.\n");
    }
  }

  Napi::Float32Array js_source_array = info[0].As<Napi::Float32Array>();
  const uint32_t channel_number = info[1].As<Napi::Number>();
  uint32_t start_in_dst_channel = 0;
  if (info.Length() > 2) {
    start_in_dst_channel = info[2].As<Napi::Number>();
  }
  if (channel_number >= audio_buffer_ptr_->number_of_channel()) {
    throw Napi_IH::RangeError::New(
        info.Env(),
        "Failed to execute 'copyFromChannel' on 'AudioBuffer': The "
        "channelNumber provided (%ld) is outside the range [0, %u].\n",
        channel_number, audio_buffer_ptr_->number_of_channel());
  }
  if (start_in_dst_channel >= audio_buffer_ptr_->length()) {
    return info.Env().Undefined();
  }

  float *source_array_ptr = js_source_array.Data();
  size_t source_array_size = js_source_array.ElementLength();
  audio_buffer_ptr_->CopyToChannel(source_array_ptr, source_array_size,
                                   channel_number, start_in_dst_channel);

  return info.Env().Undefined();
}

Napi::Value JSAudioBuffer::getChannelData(const Napi::CallbackInfo &info) {
  if (info.Length() < 1) {
    throw Napi::TypeError::New(
        info.Env(),
        "Failed to execute 'getChannelData' on 'AudioBuffer': 1 argument "
        "required, but only 0 present.\n");
  }
  const uint32_t channel = info[0].ToNumber();

  uint32_t number_of_channel = audio_buffer_ptr_->number_of_channel();
  if (channel > number_of_channel - 1) {
    throw Napi_IH::RangeError::New(
        info.Env(),
        "Failed to execute 'getChannelData' on 'AudioBuffer': channel index "
        "(%u) exceeds number of channels (%u).\n",
        channel, number_of_channel);
  }

  return js_channels_ref_[channel].Value();
}

Napi::Value JSAudioBuffer::copyFromChannel(const Napi::CallbackInfo &info) {
  if (info.Length() < 2) {
    throw Napi_IH::TypeError::New(
        info.Env(),
        "Failed to execute 'copyFromChannel' on 'AudioBuffer': 2 arguments "
        "required, but only %zu present.\n",
        info.Length());
  }

  if (!info[0].IsTypedArray()) {
    throw Napi::TypeError::New(
        info.Env(),
        "Failed to execute 'copyFromChannel' on 'AudioBuffer': parameter 1 is "
        "not of type 'Float32Array'.\n");
  } else {
    std::string constructor_name;
    Napi_IH::GetObjectConstrcutorName(info[0].As<Napi::Object>(),
                                      constructor_name);
    if (constructor_name != "Float32Array") {
      throw Napi::TypeError::New(info.Env(),
                                 "Failed to execute 'copyFromChannel' on "
                                 "'AudioBuffer': parameter 1 is "
                                 "not of type 'Float32Array'.\n");
    }
  }

  Napi::Float32Array js_dest_array = info[0].As<Napi::Float32Array>();

  float *dest_array = js_dest_array.Data();
  size_t js_dest_array_size = js_dest_array.ElementLength();
  int64_t number_of_channel = info[1].ToNumber();
  uint32_t start_idx = 0;
  if (info.Length() > 2) {
    start_idx = info[2].ToNumber();
  }

  if (number_of_channel < 0 ||
      number_of_channel >= audio_buffer_ptr_->number_of_channel()) {
    throw Napi_IH::RangeError::New(
        info.Env(),
        "Failed to execute 'copyFromChannel' on 'AudioBuffer': The "
        "channelNumber provided (%ld) is outside the range [0, %u].\n",
        number_of_channel, audio_buffer_ptr_->number_of_channel());
  }
  if (start_idx >= audio_buffer_ptr_->length()) {
    return info.Env().Undefined();
  }

  audio_buffer_ptr_->CopyFromChannel(dest_array, js_dest_array_size,
                                     number_of_channel, start_idx);

  return info.Env().Undefined();
}

}; // namespace js_audio