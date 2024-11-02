#include "JSGainNode.hpp"
#include "JSAudioParam.hpp"
#include "JSBaseAudioContext.hpp"

#include <cmath>

using GainNode = js_audio::GainNode;
using AudioNode = js_audio::AudioNode;
using GainNodeOptions = js_audio::GainNode::GainNodeOptions;
using ChannelCountMode = js_audio::AudioNode::ChannelCountMode;
using ChannelInterpretation = js_audio::AudioNode::ChannelInterpretation;
using JSBaseAudioContext = js_audio::JSBaseAudioContext;
template <typename T>
Napi_IH::FunctionWrapper (*FindClass)() = Napi_IH::IHObjectWrap::FindClass<T>;

static std::shared_ptr<GainNode>
GetGainNodeRef(const Napi_IH::IHCallbackInfo &info,
               std::shared_ptr<GainNode> gain_node_ref);
static bool ExtractOptionsFromInfo(const Napi_IH::IHCallbackInfo &info,
                                   GainNodeOptions &options,
                                   Napi::Error &napi_error);
static bool GetGainValueFromJsOptions(Napi::Env napi_env,
                                      const Napi::Object &options, float &gain,
                                      Napi::Error &napi_error);
// TODO: move to JSAudioNode protect area
static bool GetOptionsChannelCount(Napi::Env napi_env,
                                   const Napi::Object &options,
                                   uint32_t &channel_count,
                                   Napi::Error &napi_error);
static bool GetOptionsChannelCountMode(Napi::Env napi_env,
                                       const Napi::Object &options,

                                       ChannelCountMode &channel_count_mode,
                                       Napi::Error &napi_error);
static bool
GetOptionsChannelInterpretation(Napi::Env napi_env, const Napi::Object &options,
                                ChannelInterpretation &channel_interpretation,
                                Napi::Error &napi_error);

namespace js_audio {
JSGainNode::JSGainNode(const Napi_IH::IHCallbackInfo &info,
                       std::shared_ptr<GainNode> gain_node_ref)
    : JSAudioNode(info, GetGainNodeRef(info, gain_node_ref)) {
  gain_node_ptr_ = std::static_pointer_cast<GainNode>(audio_node_ptr_);
}

void JSGainNode::Init(Napi::Env env, Napi::Object exports) {
  DefineClass<JSGainNode, JSAudioNode>(
      env, "GainNode",
      {InstanceAccessor<JSGainNode, &JSGainNode::getGain>("gain")});
}

Napi::Value JSGainNode::getGain(const Napi::CallbackInfo &info) {
  if (auto gain_node_ref = gain_node_ptr_.lock()) {
    return FindClass<JSAudioParam>().NewWithArgs<JSAudioParam>(
        {}, gain_node_ref->gain());
  }
  return info.Env().Undefined();
}
} // namespace js_audio

bool ExtractOptionsFromInfo(const Napi_IH::IHCallbackInfo &info,
                            GainNodeOptions &options, Napi::Error &napi_error) {
  if (info.Length() < 2 || info[1].IsUndefined() || info[1].IsNull()) {
    return true;
  }
  if (!info[1].IsObject()) {
    napi_error = Napi::TypeError::New(
        info.Env(), "Failed to construct 'GainNode': The provided value is not "
                    "of type 'GainOptions'.\n");
    return false;
  }

  Napi::Object js_options = info[1].ToObject();

  if (!GetGainValueFromJsOptions(info.Env(), js_options, options.gain,
                                 napi_error)) {
    return false;
  }
  if (!GetOptionsChannelCount(info.Env(), js_options, options.channel_count,
                              napi_error)) {
    return false;
  }
  if (!GetOptionsChannelCountMode(info.Env(), js_options,
                                  options.channel_count_mode, napi_error)) {
    return false;
  }
  if (!GetOptionsChannelInterpretation(
          info.Env(), js_options, options.channel_interpretation, napi_error)) {
    return false;
  }

  return true;
}

static bool GetGainValueFromJsOptions(Napi::Env napi_env,
                                      const Napi::Object &js_options,
                                      float &gain, Napi::Error &napi_error) {
  napi_error.Reset();
  if (js_options.Has("gain")) {
    float maybe_gain = js_options.Get("gain").ToNumber();
    if (std::isnan(maybe_gain)) {
      napi_error = Napi::TypeError::New(
          napi_env,
          "Failed to construct 'GainNode': Failed to read the 'gain' property "
          "from 'GainOptions': The provided float value is non-finite.\n");
      return false;
    }
    gain = maybe_gain;
  }
  return true;
}

static bool GetOptionsChannelCount(Napi::Env napi_env,
                                   const Napi::Object &options,
                                   uint32_t &channel_count,
                                   Napi::Error &napi_error) {
  napi_error.Reset();
  if (options.Has("channelCount")) {
    uint32_t maybe_channel_count = options.Get("channelCount").ToNumber();

    if (!AudioNode::IsValidChannelCount(maybe_channel_count)) {
      napi_error = Napi::RangeError::New(
          napi_env, "Failed to construct 'GainNode': The channel count "
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

static bool GetOptionsChannelCountMode(Napi::Env napi_env,
                                       const Napi::Object &options,
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
          "Failed to construct 'GainNode': Failed to read the "
          "'channelCountMode' property from 'AudioNodeOptions': The provided "
          "value '" +
              str_channel_count_mode +
              "' is not a valid enum value of type ChannelCountMode.\n");
      return false;
    }
  }
  return true;
}

static bool
GetOptionsChannelInterpretation(Napi::Env napi_env, const Napi::Object &options,
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
          "Failed to construct 'GainNode': Failed to read the "
          "'channelInterpretation' property from 'AudioNodeOptions': The "
          "provided value '" +
              str_channel_interpretation +
              "' is not a valid enum value of type ChannelInterpretation.\n");
      return false;
    }
  }
  return true;
}

static std::shared_ptr<GainNode>
GetGainNodeRef(const Napi_IH::IHCallbackInfo &info,
               std::shared_ptr<GainNode> gain_node_ref) {
  if (gain_node_ref) {
    return gain_node_ref;
  }
  if (info.Length() < 1) {
    throw Napi_IH::TypeError::New(info.Env(),
                                  "Failed to construct 'GainNode': 1 argument "
                                  "required, but only 0 present.");
  }
  if (!info[0].IsObject()) {
    throw Napi::TypeError::New(
        info.Env(), "Failed to construct 'GainNode': parameter 1 is not "
                    "of type 'BaseAudioContext'.\n");
  }

  Napi::Object js_base_audio_context = info[0].As<Napi::Object>();
  if (!js_base_audio_context.InstanceOf(
          FindClass<JSBaseAudioContext>().InnerFunction())) {
    throw Napi::TypeError::New(info.Env(),
                               "Failed to construct 'GainNode': parameter 1 is "
                               "not of type 'BaseAudioContext'.\n");
  }

  JSBaseAudioContext *js_base_audio_context_ptr =
      Napi_IH::IHObjectWrap::UnWrap<JSBaseAudioContext>(js_base_audio_context);
  std::shared_ptr<std::mutex> audio_context_lock =
      js_base_audio_context_ptr->GetAudioContextLock();

  GainNodeOptions options = GainNode::GetDefaultOptions();
  Napi::Error napi_error;
  if (!ExtractOptionsFromInfo(info, options, napi_error)) {
    throw napi_error;
  }

  return GainNode::CreateGain(options, audio_context_lock);
}
