#include <functional>

#include "JSAudioParam.hpp"
#include "JSBaseAudioContext.hpp"
#include "JSOscillatorNode.hpp"

using OscillatorNode = js_audio::OscillatorNode;
using JSBaseAudioContext = js_audio::JSBaseAudioContext;
using OscillatorNode = js_audio::OscillatorNode;
using OscillatorOptions = OscillatorNode::OscillatorOptions;
using OscillatorType = OscillatorNode::OscillatorType;
using AudioNode = js_audio::AudioNode;
using ChannelCountMode = AudioNode::ChannelCountMode;
using ChannelInterpretation = AudioNode::ChannelInterpretation;

struct ErrorWrapper {
  Napi::Error error;
  std::string msg;
};

template <typename T>
Napi_IH::FunctionWrapper (*FindClass)() = Napi_IH::IHObjectWrap::FindClass<T>;

static bool GetOptionsType(const Napi::Object &options, Napi::Env napi_env,
                           OscillatorType &type, Napi::Error &napi_error);

static bool GetOptionsDetune(const Napi::Object &options, Napi::Env napi_env,
                             float &detune, Napi::Error &napi_error);

static bool GetOptionsFrequency(const Napi::Object &options, Napi::Env napi_env,
                                float &frequency, const float &sample_rate,
                                Napi::Error &napi_error);

static bool GetOptionsChannelCount(const Napi::Object &options,
                                   Napi::Env napi_env, uint32_t &channel_count,
                                   Napi::Error &napi_error);

static bool GetOptionsChannelCountMode(const Napi::Object &options,
                                       Napi::Env napi_env,
                                       ChannelCountMode &channel_count_mode,
                                       Napi::Error &napi_error);

static bool
GetOptionsChannelInterpretation(const Napi::Object &options, Napi::Env napi_env,
                                ChannelInterpretation &channel_interpretation,
                                Napi::Error &napi_error);

static bool GetOptionsParams(const Napi_IH::IHCallbackInfo &info,
                             const float &sample_rate,
                             OscillatorOptions &osc_options,
                             Napi::Error &napi_error) {
  if (info.Length() < 2 || info[1].IsNull() || info[1].IsUndefined()) {
    // options object is optional
    LOGW("Info only has one element!\n");
    return true;
  }

  if (!info[1].IsObject()) {
    napi_error = Napi::TypeError::New(
        info.Env(), "Failed to construct 'OscillatorNode': The provided value "
                    "is not of type 'OscillatorOptions'.\n");
    return false;
  }

  Napi::Object options = info[1].As<Napi::Object>();
  if (!GetOptionsType(options, info.Env(), osc_options.type, napi_error)) {
    return false;
  }
  if (!GetOptionsDetune(options, info.Env(), osc_options.detune, napi_error)) {
    return false;
  }
  if (!GetOptionsFrequency(options, info.Env(), osc_options.frequency,
                           sample_rate, napi_error)) {
    return false;
  }
  if (!GetOptionsChannelCount(options, info.Env(), osc_options.channel_count,
                              napi_error)) {
    return false;
  }
  if (!GetOptionsChannelCountMode(options, info.Env(),
                                  osc_options.channel_count_mode, napi_error)) {
    return false;
  }
  if (!GetOptionsChannelInterpretation(options, info.Env(),
                                       osc_options.channel_interpretation,
                                       napi_error)) {
    return false;
  }
  return true;
}

static std::shared_ptr<OscillatorNode> GetOscillatorNodeRef(
    const Napi_IH::IHCallbackInfo &info,
    std::shared_ptr<OscillatorNode> oscillator_node_ref = nullptr) {
  if (oscillator_node_ref) {
    return oscillator_node_ref;
  }
  if (info.Length() < 1) {
    throw Napi::TypeError::New(info.Env(),
                               "Failed to construct 'OscillatorNode': 1 "
                               "argument required, but only 0 present.\n");
  }

  if (!info[0].IsObject()) {
    throw Napi::TypeError::New(
        info.Env(), "Failed to construct 'OscillatorNode': parameter 1 is not "
                    "of type 'BaseAudioContext'.\n");
  }

  Napi::Object js_base_audio_context = info[0].As<Napi::Object>();

  if (!js_base_audio_context.InstanceOf(
          FindClass<JSBaseAudioContext>().InnerFunction())) {
    throw Napi::TypeError::New(
        info.Env(), "Failed to construct 'OscillatorNode': parameter 1 is not "
                    "of type 'BaseAudioContext'.\n");
  }

  JSBaseAudioContext *js_base_audio_context_ptr =
      Napi_IH::IHObjectWrap::UnWrap<JSBaseAudioContext>(js_base_audio_context);
  std::shared_ptr<std::mutex> audio_context_lock_ref =
      js_base_audio_context_ptr->GetAudioContextLock();
  float sample_rate = js_base_audio_context_ptr->GetSampleRate();

  OscillatorOptions osc_options = OscillatorNode::GetDefaultOptions();
  Napi::Error napi_error;

  if (info.Length() >= 2) {
    if (!GetOptionsParams(info, sample_rate, osc_options, napi_error)) {
      if (!napi_error.IsEmpty()) {
        throw napi_error;
      }
    }
  }

  return OscillatorNode::CreateOscillatorNode(audio_context_lock_ref,
                                              osc_options, sample_rate);
}

namespace js_audio {
JSOscillatorNode::JSOscillatorNode(
    const Napi_IH::IHCallbackInfo &info,
    std::shared_ptr<OscillatorNode> oscillator_node_ref)
    : JSAudioScheduledSourceNode(
          info, GetOscillatorNodeRef(info, oscillator_node_ref)) {
  oscillator_node_ptr_ =
      std::static_pointer_cast<OscillatorNode>(audio_node_ref_);
}

void JSOscillatorNode::Init(Napi::Env env, Napi::Object exports) {
  DefineClass<JSOscillatorNode, JSAudioScheduledSourceNode>(
      env, "OscillatorNode",
      {
          InstanceAccessor<JSOscillatorNode, &JSOscillatorNode::getFrequency>(
              "frequency"),
          InstanceAccessor<JSOscillatorNode, &JSOscillatorNode::getDetune>(
              "detune"),
          InstanceAccessor<JSOscillatorNode, &JSOscillatorNode::getType,
                           &JSOscillatorNode::setType>("type"),
      });
}

void JSOscillatorNode::setType(const Napi::CallbackInfo &info,
                               const Napi::Value &value) {
  if (auto oscillator_node_ref = oscillator_node_ptr_.lock()) {
    if (!value.IsString()) {
      LOGE("Error! Type of oscillator node should be a string value!\n");
    }
    oscillator_node_ref->set_type(value.As<Napi::String>());
  } else {
    LOGE("Error! Inner oscillator node expired!\n");
  }
}

Napi::Value JSOscillatorNode::getFrequency(const Napi::CallbackInfo &info) {
  if (auto oscillator_node_ref = oscillator_node_ptr_.lock()) {

    Napi::Object js_frequency =
        FindClass<JSAudioParam>().NewWithArgs<JSAudioParam>(
            {info.This()}, oscillator_node_ref->frequency_ref());
    return js_frequency;
  }
  return info.Env().Undefined();
}

Napi::Value JSOscillatorNode::getDetune(const Napi::CallbackInfo &info) {
  if (auto oscillator_node_ref = oscillator_node_ptr_.lock()) {
    Napi::Object js_detune =
        FindClass<JSAudioParam>().NewWithArgs<JSAudioParam>(
            {info.This()}, oscillator_node_ref->detune_ref());
    return js_detune;
  }
  return info.Env().Undefined();
}

Napi::Value JSOscillatorNode::getType(const Napi::CallbackInfo &info) {
  if (auto oscillator_node_ref = oscillator_node_ptr_.lock()) {
    Napi::String type_value =
        Napi::String::New(info.Env(), oscillator_node_ref->type());
    return type_value;
  }
  return info.Env().Undefined();
}

} // namespace js_audio

static bool GetOptionsType(const Napi::Object &options, Napi::Env napi_env,
                           OscillatorType &type, Napi::Error &napi_error) {
  napi_error.Reset();
  if (options.Has("type")) {
    std::string str_type = options.Get("type").ToString();
    if (!OscillatorNode::ConvertToOscillatorType(str_type, type)) {
      napi_error = Napi::TypeError::New(
          napi_env,
          "Failed to construct 'OscillatorNode': Failed to read the 'type' "
          "property from 'OscillatorOptions': The provided value '" +
              str_type + "' is not a valid enum value of type OscillatorType.");
      return false;
    }
  }
  return true;
}

static bool GetOptionsDetune(const Napi::Object &options, Napi::Env napi_env,
                             float &detune, Napi::Error &napi_error) {
  napi_error.Reset();
  if (options.Has("detune")) {
    float maybe_detune = options.Get("detune").ToNumber();
    if (std::isnan(maybe_detune)) {
      napi_error = Napi::TypeError::New(
          napi_env, "Failed to construct 'OscillatorNode': Failed to read the "
                    "'detune' property from 'OscillatorOptions': The provided "
                    "float value is non-finite.\n");
      return false;
    }
    detune = OscillatorNode::ClampToValidDetune(maybe_detune);
  }
  return true;
}

static bool GetOptionsFrequency(const Napi::Object &options, Napi::Env napi_env,
                                float &frequency, const float &sample_rate,
                                Napi::Error &napi_error) {
  napi_error.Reset();
  if (options.Has("frequency")) {
    float maybe_frequency = options.Get("frequency").ToNumber();
    if (std::isnan(maybe_frequency)) {
      napi_error = Napi::TypeError::New(
          napi_env,
          "Failed to construct 'OscillatorNode': Failed to read the "
          "'frequency' property from 'OscillatorOptions': The provided "
          "float value is non-finite.\n");
      return false;
    }
    frequency = std::clamp(maybe_frequency, -sample_rate / 2, sample_rate / 2);
  }
  return true;
}

static bool GetOptionsChannelCount(const Napi::Object &options,
                                   Napi::Env napi_env, uint32_t &channel_count,
                                   Napi::Error &napi_error) {
  napi_error.Reset();
  if (options.Has("channelCount")) {
    uint32_t maybe_channel_count = options.Get("channelCount").ToNumber();

    if (!AudioNode::IsValidChannelCount(maybe_channel_count)) {
      napi_error = Napi::RangeError::New(
          napi_env, "Failed to construct 'OscillatorNode': The channel count "
                    "provided (" +
                        std::to_string(maybe_channel_count) +
                        " ) is outside the range [" +
                        std::to_string(AudioNode::kMinChannelCount) + ", " +
                        std::to_string(AudioNode::kMaxChannelCount) + "].\n");
      return false;
    }
    channel_count = maybe_channel_count;
  }
  return true;
}

static bool GetOptionsChannelCountMode(const Napi::Object &options,
                                       Napi::Env napi_env,
                                       ChannelCountMode &channel_count_mode,
                                       Napi::Error &napi_error) {
  napi_error.Reset();
  if (options.Has("channelCountMode")) {
    std::string str_channel_count_mode =
        options.Get("channelCountMode").ToString();
    if (!AudioNode::ConvertToChannelCountMode(str_channel_count_mode,
                                              channel_count_mode)) {
      napi_error = Napi::TypeError::New(
          napi_env,
          "Failed to construct 'OscillatorNode': Failed to read the "
          "'channelCountMode' property from 'AudioNodeOptions': The "
          "provided value '" +
              str_channel_count_mode +
              "' is not a valid enum value of type ChannelCountMode.\n");
      return false;
    }
  }
  return true;
}

static bool
GetOptionsChannelInterpretation(const Napi::Object &options, Napi::Env napi_env,
                                ChannelInterpretation &channel_interpretation,
                                Napi::Error &napi_error) {
  napi_error.Reset();
  if (options.Has("channelInterpretation")) {
    std::string str_channel_interpretation =
        options.Get("channelInterpretation").ToString();
    if (!AudioNode::ConvertToChannelInterpretation(str_channel_interpretation,
                                                   channel_interpretation)) {
      napi_error = Napi::TypeError::New(
          napi_env,
          "Failed to construct 'OscillatorNode': Failed to read the "
          "'channelInterpretation' property from 'AudioNodeOptions': The "
          "provided value '" +
              str_channel_interpretation +
              "' is not a valid enum value of type ChannelInterpretation.\n");
      return false;
    }
  }
  return true;
}
