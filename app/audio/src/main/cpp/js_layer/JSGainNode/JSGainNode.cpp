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
using JSAudioNode = js_audio::JSAudioNode;

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

namespace js_audio {
JSGainNode::JSGainNode(const Napi_IH::IHCallbackInfo &info,
                       std::shared_ptr<GainNode> gain_node_ref)
    : JSAudioNode(info, GetGainNodeRef(info, gain_node_ref)) {
  gain_node_ptr_ = std::static_pointer_cast<GainNode>(audio_node_ref_);
}

void JSGainNode::Init(Napi::Env env, Napi::Object exports) {
  DefineClass<JSGainNode, JSAudioNode>(
      env, "GainNode",
      {InstanceAccessor<JSGainNode, &JSGainNode::getGain>("gain")});
}

Napi::Value JSGainNode::getGain(const Napi::CallbackInfo &info) {
  if (auto gain_node_ref = gain_node_ptr_.lock()) {
    return FindClass<JSAudioParam>().NewWithArgs<JSAudioParam>(
        {}, gain_node_ref->gain_ref());
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
  if (!JSAudioNode::ExtractOptionsFromInfo("Gain", info, options, napi_error)) {
    return false;
  }

  Napi::Object js_options = info[1].ToObject();

  if (!GetGainValueFromJsOptions(info.Env(), js_options, options.gain,
                                 napi_error)) {
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
  std::shared_ptr<std::mutex> audio_context_lock_ref =
      js_base_audio_context_ptr->GetAudioContextLock();

  GainNodeOptions options = GainNode::GetDefaultOptions();
  Napi::Error napi_error;
  if (!ExtractOptionsFromInfo(info, options, napi_error)) {
    throw napi_error;
  }

  return GainNode::CreateGainNode(options, audio_context_lock_ref);
}
