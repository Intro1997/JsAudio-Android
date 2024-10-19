#include "JSAudioNode.hpp"
#include "JSBaseAudioContext.hpp"
#include "logger.hpp"

namespace js_audio {

JSAudioNode::JSAudioNode(const Napi_IH::IHCallbackInfo &info,
                         std::shared_ptr<AudioNode> audio_node_ptr)
    : Napi_IH::IHObjectWrap(info), audio_node_ptr_(audio_node_ptr) {
  if (info.Length() < 1 || !info[0].IsObject()) {
    LOGE("Error: Cannot create AudioNode without AudioContext!\n");
    return;
  }

  napi_audio_context_ref_ = Napi::Weak(info[0].As<Napi::Object>());
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

Napi::Value JSAudioNode::getContext(const Napi::CallbackInfo &info) {
  Napi::Object napi_audio_context = napi_audio_context_ref_.Value();
  if (napi_audio_context.IsEmpty()) {
    LOGW("Warn: the context of AudioNode is expired!\n");
  }

  return napi_audio_context;
}

Napi::Value JSAudioNode::getNumberOfInputs(const Napi::CallbackInfo &info) {
  if (!audio_node_ptr_) {
    LOGE("Error! Inner audio node ptr is nullptr!\n");
    return info.Env().Undefined();
  }
  return Napi::Value::From(info.Env(), audio_node_ptr_->number_of_inputs());
}

Napi::Value JSAudioNode::getNumberOfOutputs(const Napi::CallbackInfo &info) {
  if (!audio_node_ptr_) {
    LOGE("Error! Inner audio node ptr is nullptr!\n");
    return info.Env().Undefined();
  }
  return Napi::Value::From(info.Env(), audio_node_ptr_->number_of_outputs());
}

Napi::Value JSAudioNode::getChannelCount(const Napi::CallbackInfo &info) {
  if (!audio_node_ptr_) {
    LOGE("Error! Inner audio node ptr is nullptr!\n");
    return info.Env().Undefined();
  }
  return Napi::Value::From(info.Env(), audio_node_ptr_->channel_count());
}

Napi::Value JSAudioNode::getChannelCountMode(const Napi::CallbackInfo &info) {
  if (!audio_node_ptr_) {
    LOGE("Error! Inner audio node ptr is nullptr!\n");
    return info.Env().Undefined();
  }

  return Napi::String::New(info.Env(),
                           AudioNode::ConvertChannelCountModeToString(
                               audio_node_ptr_->channel_count_mode()));
}

Napi::Value
JSAudioNode::getChannelInterpretation(const Napi::CallbackInfo &info) {
  if (!audio_node_ptr_) {
    LOGE("Error! Inner audio node ptr is nullptr!\n");
    return info.Env().Undefined();
  }
  return Napi::String::New(info.Env(),
                           AudioNode::ConvertChannelInterpretationToString(
                               audio_node_ptr_->channel_interpretation()));
}

Napi::Value JSAudioNode::connect(const Napi::CallbackInfo &info) {
  if (!audio_node_ptr_) {
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

  if (!js_dest_audio_node_ptr || !js_dest_audio_node_ptr->audio_node_ptr_) {
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

  audio_node_ptr_->ConnectTo(js_dest_audio_node_ptr->audio_node_ptr_);
  js_dest_audio_node_ptr->audio_node_ptr_->BeConnectedTo(audio_node_ptr_);

  return napi_dest_audio_node_obj;
}

Napi::Value JSAudioNode::disconnect(const Napi::CallbackInfo &info) {
  if (!audio_node_ptr_) {
    LOGE("Error! Inner audio node ptr is nullptr!\n");
  } else {
    audio_node_ptr_->Disconnect();
  }
  return info.Env().Undefined();
}

} // namespace js_audio