#include "JSBaseAudioContext.hpp"
#include "AudioEngine.hpp"
#include "JSAudioDestinationNode.hpp"

namespace js_audio {

JSBaseAudioContext::JSBaseAudioContext(
    const Napi_IH::IHCallbackInfo &info,
    std::shared_ptr<BaseAudioContext> base_audio_context_ptr)
    : Napi_IH::IHObjectWrap(info) {
  if (base_audio_context_ptr) {
    base_audio_context_ptr_ = base_audio_context_ptr;
  } else {
    // Can't get in here when use inheritance
    base_audio_context_ptr_ = std::make_shared<BaseAudioContext>();
  }

  Napi::Env env = info.Env();

  Napi::Object js_destination_node =
      JSAudioDestinationNode::FindClass<JSAudioDestinationNode>()
          .NewWithArgs<JSAudioDestinationNode>(
              {
                  Napi::Number::From(
                      env,
                      1), // TODO: Use default value of specification
                  Napi::Number::From(
                      env,
                      0), // TODO: Use default value of specification
                  Napi::Number::From(
                      env,
                      2), // TODO: Use default value of specification
              },
              base_audio_context_ptr_->audio_destination_node_ptr());
  js_destination_node_ref_ = Napi::Persistent(js_destination_node);

  bool is_add_to_audio_player = false;
  if (auto audio_engine_ptr = AudioEngine::Get().lock()) {
    if (auto audio_player_ptr =
            audio_engine_ptr
                ->GetAudioPlayer(AudioPlayerType::kBufferQueuePlayer)
                .lock()) {
      audio_player_ptr->AddBaseAudioContext(base_audio_context_ptr_);
      is_add_to_audio_player = true;
    }
  }

  if (!is_add_to_audio_player) {
    // we do not need to reserve cpp pointer
    // when add to audio player failed
    base_audio_context_ptr_.reset();
    js_destination_node_ref_.Reset();
    LOGE("Add Base Audio Context to Audio Player failed!\n");
  }
}

void JSBaseAudioContext::Init(Napi::Env env, Napi::Object exports) {
  DefineClass<JSBaseAudioContext>(
      env, "BaseAudioContext",
      {
          InstanceAccessor<JSBaseAudioContext,
                           &JSBaseAudioContext::GetDestinationNode>(
              "destination"),
          InstanceAccessor<JSBaseAudioContext,
                           &JSBaseAudioContext::GetSampleRate>("sampleRate"),
          InstanceAccessor<JSBaseAudioContext,
                           &JSBaseAudioContext::GetCurrentTime>("currentTime"),
          InstanceMethod<JSBaseAudioContext,
          InstanceMethod<JSBaseAudioContext, &JSBaseAudioContext::createBuffer>(
              "createBuffer"),
      },
      false);
}

Napi::Value
JSBaseAudioContext::GetDestinationNode(const Napi::CallbackInfo &info) {
  return js_destination_node_ref_.Value();
}

Napi::Value JSBaseAudioContext::GetSampleRate(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  return Napi::Number::From(env, base_audio_context_ptr_->sample_rate());
}

Napi::Value JSBaseAudioContext::GetCurrentTime(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  return Napi::Number::From(env, base_audio_context_ptr_->GetCurrentTime());
}
Napi::Value JSBaseAudioContext::createBuffer(const Napi::CallbackInfo &info) {
  if (info.Length() < 3) {
    LOGE("Create JSAudioBuffer failed! Need 3 parameters, but get %zu.\n",
         info.Length());
    return info.Env().Undefined();
  }
  if (info[0].IsNumber() && info[1].IsNumber() && info[2].IsNumber()) {
    Napi::Object js_audio_buffer =
        JSAudioBuffer::FindClass<JSAudioBuffer>().NewWithArgs<JSAudioBuffer>(
            {info[0], info[1], info[2]});

    return js_audio_buffer;
  } else {
    LOGE("Create JSAudioBuffer failed! Need 3 number parameters.\n");
  }
  return info.Env().Undefined();
}
} // namespace js_audio