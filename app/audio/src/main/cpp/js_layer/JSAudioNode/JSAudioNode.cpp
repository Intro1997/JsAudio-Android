#include "JSAudioNode.hpp"
#include "JSBaseAudioContext.hpp"
#include "logger.hpp"

using AudioNode = js_audio::AudioNode;
using ChannelCountMode = js_audio::AudioNode::ChannelCountMode;
using ChannelInterpretation = js_audio::AudioNode::ChannelInterpretation;

static bool GetOptionsChannelCount(const std::string &audio_node_type_name,
                                   Napi::Env napi_env,
                                   const Napi::Object &options,
                                   uint32_t &channel_count,
                                   Napi::Error &napi_error);
static bool GetOptionsChannelCountMode(const std::string &audio_node_type_name,
                                       Napi::Env napi_env,
                                       const Napi::Object &options,
                                       ChannelCountMode &channel_count_mode,
                                       Napi::Error &napi_error);
static bool
GetOptionsChannelInterpretation(const std::string &audio_node_type_name,
                                Napi::Env napi_env, const Napi::Object &options,
                                ChannelInterpretation &channel_interpretation,
                                Napi::Error &napi_error);
namespace js_audio {

JSAudioNode::JSAudioNode(const Napi_IH::IHCallbackInfo &info,
                         std::shared_ptr<AudioNode> audio_node_ref)
    : Napi_IH::IHObjectWrap(info), audio_node_ref_(audio_node_ref) {
  if (info.Length() < 1 || !info[0].IsObject()) {
    LOGE("Error: Cannot create AudioNode without AudioContext!\n");
    return;
  }

  napi_audio_context_ref_ = Napi::Weak(info[0].As<Napi::Object>());
  JSBaseAudioContext *js_base_audio_context_ptr =
      UnWrap<JSBaseAudioContext>(napi_audio_context_ref_.Value());
  if (js_base_audio_context_ptr) {
    audio_node_ref_->base_audio_context_ptr_ =
        js_base_audio_context_ptr->GetAudioContext();
  }
}

void JSAudioNode::Init(Napi::Env env, Napi::Object exports) {
  DefineClass<JSAudioNode>(
      env, "AudioNode",
      {InstanceAccessor<JSAudioNode, &JSAudioNode::getContext>("context"),
       InstanceAccessor<JSAudioNode, &JSAudioNode::getNumberOfInputs>(
           "numberOfInputs"),
       InstanceAccessor<JSAudioNode, &JSAudioNode::getNumberOfOutputs>(
           "numberOfOutputs"),
       InstanceAccessor<JSAudioNode, &JSAudioNode::getChannelCount>(
           "channelCount"),
       InstanceAccessor<JSAudioNode, &JSAudioNode::getChannelCountMode>(
           "channelCountMode"),
       InstanceAccessor<JSAudioNode, &JSAudioNode::getChannelInterpretation>(
           "channelInterpretation"),
       InstanceMethod<JSAudioNode, &JSAudioNode::connect>("connect"),
       InstanceMethod<JSAudioNode, &JSAudioNode::disconnect>("disconnect")},
      Napi_IH::ClassVisibility::kHideConstructor);
}

/**
 * audio_node_type_name: Type of AudioNode, used to print error info. For
 * example, Gain is type of GainNode;
 */
bool JSAudioNode::ExtractOptionsFromInfo(
    const std::string &audio_node_type_name,
    const Napi_IH::IHCallbackInfo &info, AudioNode::AudioNodeOptions &options,
    Napi::Error &napi_error) {
  if (info.Length() < 2 || info[1].IsUndefined() || info[1].IsNull()) {
    return true;
  }
  if (!info[1].IsObject()) {
    napi_error = Napi_IH::TypeError::New(
        info.Env(),
        "Failed to construct '%sNode': The provided value is not "
        "of type '%sOptions'.\n",
        audio_node_type_name.c_str(), audio_node_type_name.c_str());
    return false;
  }

  Napi::Object js_options = info[1].ToObject();

  if (!GetOptionsChannelCount(audio_node_type_name, info.Env(), js_options,
                              options.channel_count, napi_error)) {
    return false;
  }
  if (!GetOptionsChannelCountMode(audio_node_type_name, info.Env(), js_options,
                                  options.channel_count_mode, napi_error)) {
    return false;
  }
  if (!GetOptionsChannelInterpretation(
          audio_node_type_name, info.Env(), js_options,
          options.channel_interpretation, napi_error)) {
    return false;
  }

  return true;
}

Napi::Value JSAudioNode::getContext(const Napi::CallbackInfo &info) {
  Napi::Object napi_audio_context = napi_audio_context_ref_.Value();
  if (napi_audio_context.IsEmpty()) {
    LOGW("Warn: the context of AudioNode is expired!\n");
  }

  return napi_audio_context;
}

Napi::Value JSAudioNode::getNumberOfInputs(const Napi::CallbackInfo &info) {
  if (!audio_node_ref_) {
    LOGE("Error! Inner audio node ptr is nullptr!\n");
    return info.Env().Undefined();
  }
  return Napi::Value::From(info.Env(), audio_node_ref_->number_of_inputs());
}

Napi::Value JSAudioNode::getNumberOfOutputs(const Napi::CallbackInfo &info) {
  if (!audio_node_ref_) {
    LOGE("Error! Inner audio node ptr is nullptr!\n");
    return info.Env().Undefined();
  }
  return Napi::Value::From(info.Env(), audio_node_ref_->number_of_outputs());
}

Napi::Value JSAudioNode::getChannelCount(const Napi::CallbackInfo &info) {
  if (!audio_node_ref_) {
    LOGE("Error! Inner audio node ptr is nullptr!\n");
    return info.Env().Undefined();
  }
  return Napi::Value::From(info.Env(), audio_node_ref_->channel_count());
}

Napi::Value JSAudioNode::getChannelCountMode(const Napi::CallbackInfo &info) {
  if (!audio_node_ref_) {
    LOGE("Error! Inner audio node ptr is nullptr!\n");
    return info.Env().Undefined();
  }

  return Napi::String::New(info.Env(),
                           AudioNode::ConvertChannelCountModeToString(
                               audio_node_ref_->channel_count_mode()));
}

Napi::Value
JSAudioNode::getChannelInterpretation(const Napi::CallbackInfo &info) {
  if (!audio_node_ref_) {
    LOGE("Error! Inner audio node ptr is nullptr!\n");
    return info.Env().Undefined();
  }
  return Napi::String::New(info.Env(),
                           AudioNode::ConvertChannelInterpretationToString(
                               audio_node_ref_->channel_interpretation()));
}

Napi::Value JSAudioNode::connect(const Napi::CallbackInfo &info) {
  if (!audio_node_ref_) {
    LOGE("Error! Inner audio node ptr is nullptr!\n");
    return info.Env().Undefined();
  }

  if (info.Length() < 1 || !info[0].IsObject()) {
    LOGE("Error! connect function need one AudioNode object parameter\n");
    return info.Env().Undefined();
  }

  Napi::Object napi_dest_audio_node_obj = info[0].As<Napi::Object>();
  JSAudioNode *js_dest_audio_node_ptr =
      UnWrap<JSAudioNode>(napi_dest_audio_node_obj);

  if (!js_dest_audio_node_ptr || !js_dest_audio_node_ptr->audio_node_ref_) {
    LOGE("Error! Try to connect an invalid AudioNode\n");
    return info.Env().Undefined();
  }

  JSBaseAudioContext *js_base_audio_ctx_ptr =
      UnWrap<JSBaseAudioContext>(napi_audio_context_ref_.Value());
  JSBaseAudioContext *other_js_base_audio_ctx_ptr = UnWrap<JSBaseAudioContext>(
      js_dest_audio_node_ptr->napi_audio_context_ref_.Value());

  if (!js_base_audio_ctx_ptr) {
    LOGE("Error: the context of AudioNode is expired!\n");
    return info.Env().Undefined();
  }

  if (!other_js_base_audio_ctx_ptr) {
    LOGE("Error: the context of destination AudioNode is expired!\n");
    return info.Env().Undefined();
  }

  if (js_base_audio_ctx_ptr != other_js_base_audio_ctx_ptr) {
    LOGE("Error! Cannot connect to the AudioNode of different AudioContext!\n");
    return info.Env().Undefined();
  }

  audio_node_ref_->ConnectTo(js_dest_audio_node_ptr->audio_node_ref_);
  js_dest_audio_node_ptr->audio_node_ref_->BeConnectedTo(audio_node_ref_);

  return napi_dest_audio_node_obj;
}

Napi::Value JSAudioNode::disconnect(const Napi::CallbackInfo &info) {
  if (!audio_node_ref_) {
    LOGE("Error! Inner audio node ptr is nullptr!\n");
  } else {
    audio_node_ref_->Disconnect();
  }
  return info.Env().Undefined();
}

} // namespace js_audio

bool GetOptionsChannelCount(const std::string &audio_node_type_name,
                            Napi::Env napi_env, const Napi::Object &options,
                            uint32_t &channel_count, Napi::Error &napi_error) {
  napi_error.Reset();
  if (options.Has("channelCount")) {
    uint32_t maybe_channel_count = options.Get("channelCount").ToNumber();

    if (!AudioNode::IsValidChannelCount(maybe_channel_count)) {
      napi_error = Napi_IH::RangeError::New(
          napi_env,
          "Failed to construct '%sNode': The channel count provided (%u) is "
          "outside the range [%u, %u].\n",
          audio_node_type_name.c_str(), maybe_channel_count,
          AudioNode::kMinChannelCount, AudioNode::kMaxChannelCount);
      return false;
    }
    channel_count = maybe_channel_count;
  }
  return true;
}

bool GetOptionsChannelCountMode(const std::string &audio_node_type_name,
                                Napi::Env napi_env, const Napi::Object &options,
                                ChannelCountMode &channel_count_mode,
                                Napi::Error &napi_error) {
  napi_error.Reset();
  if (options.Has("channelCountMode")) {
    std::string str_channel_count_mode =
        options.Get("channelCountMode").ToString();
    if (!AudioNode::ConvertToChannelCountMode(str_channel_count_mode,
                                              channel_count_mode)) {
      napi_error = Napi_IH::TypeError::New(
          napi_env,
          "Failed to construct '%sNode': Failed to read the 'channelCountMode' "
          "property from 'AudioNodeOptions': The provided value '%s' is not a "
          "valid enum value of type ChannelCountMode.\n",
          audio_node_type_name.c_str(), str_channel_count_mode.c_str());
      return false;
    }
  }
  return true;
}

bool GetOptionsChannelInterpretation(
    const std::string &audio_node_type_name, Napi::Env napi_env,
    const Napi::Object &options, ChannelInterpretation &channel_interpretation,
    Napi::Error &napi_error) {
  napi_error.Reset();
  if (options.Has("channelInterpretation")) {
    std::string str_channel_interpretation =
        options.Get("channelInterpretation").ToString();
    if (!AudioNode::ConvertToChannelInterpretation(str_channel_interpretation,
                                                   channel_interpretation)) {
      napi_error = Napi_IH::TypeError::New(
          napi_env,
          "Failed to construct '%sNode': Failed to read the "
          "'channelInterpretation' property from 'AudioNodeOptions': The "
          "provided value '%s' is not a valid enum value of type "
          "ChannelInterpretation.\n",
          audio_node_type_name.c_str(), str_channel_interpretation.c_str());
      return false;
    }
  }
  return true;
}
