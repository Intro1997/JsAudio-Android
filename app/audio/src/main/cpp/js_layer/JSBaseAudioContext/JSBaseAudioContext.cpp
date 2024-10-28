#include "JSBaseAudioContext.hpp"
#include "AudioEngine.hpp"
#include "JSAudioBuffer.hpp"
#include "JSAudioDestinationNode.hpp"
#include "JSOscillatorNode.hpp"
#include "OscillatorNode.hpp"

namespace js_audio {

JSBaseAudioContext::JSBaseAudioContext(
    const Napi_IH::IHCallbackInfo &info,
    std::shared_ptr<BaseAudioContext> base_audio_context_ptr)
    : Napi_IH::IHObjectWrap(info) {
  if (base_audio_context_ptr) {
    base_audio_context_ptr_ = base_audio_context_ptr;
  } else {
    // Shouldn't get in here when use inheritance
    base_audio_context_ptr_ = std::make_shared<BaseAudioContext>();
  }

  Napi::Object js_destination_node =
      JSAudioDestinationNode::FindClass<JSAudioDestinationNode>()
          .NewWithArgs<JSAudioDestinationNode>(
              {info.This()},
              base_audio_context_ptr_->audio_destination_node_ptr());

  js_destination_node_ref_ = Napi::Persistent(js_destination_node);

  bool is_add_to_audio_player = false;
  auto audio_engine_ptr = AudioEngine::Get().lock();
  if (audio_engine_ptr && base_audio_context_ptr_->IsOnlineContext()) {
    if (auto audio_player_ptr =
            audio_engine_ptr
                ->GetAudioPlayer(AudioPlayerType::kBufferQueuePlayer)
                .lock()) {
      is_add_to_audio_player =
          audio_player_ptr->AddBaseAudioContext(base_audio_context_ptr_);
    }
  }

  if (!is_add_to_audio_player && base_audio_context_ptr_->IsOnlineContext()) {
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
                           &JSBaseAudioContext::getDestinationNode>(
              "destination"),
          InstanceAccessor<JSBaseAudioContext,
                           &JSBaseAudioContext::getSampleRate>("sampleRate"),
          InstanceAccessor<JSBaseAudioContext,
                           &JSBaseAudioContext::getCurrentTime>("currentTime"),
          InstanceMethod<JSBaseAudioContext,
                         &JSBaseAudioContext::createOscillator>(
              "createOscillator"),
          InstanceMethod<JSBaseAudioContext, &JSBaseAudioContext::createBuffer>(
              "createBuffer"),
      },
      Napi_IH::ClassVisibility::kHideConstructor);
}

Napi::Value
JSBaseAudioContext::getDestinationNode(const Napi::CallbackInfo &info) {
  return js_destination_node_ref_.Value();
}

Napi::Value JSBaseAudioContext::getSampleRate(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  return Napi::Number::From(env, base_audio_context_ptr_->sample_rate());
}

Napi::Value JSBaseAudioContext::getCurrentTime(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  return Napi::Number::From(env, base_audio_context_ptr_->GetCurrentTime());
}

std::shared_ptr<std::mutex> JSBaseAudioContext::GetAudioContextLock() const {
  return base_audio_context_ptr_->GetLock();
}

std::shared_ptr<BaseAudioContext> JSBaseAudioContext::GetAudioContext() const {
  return base_audio_context_ptr_;
}

float JSBaseAudioContext::GetSampleRate() const {
  return base_audio_context_ptr_->sample_rate();
}

Napi::Value
JSBaseAudioContext::createOscillator(const Napi::CallbackInfo &info) {
  std::shared_ptr<OscillatorNode> oscillator_node_ptr =
      OscillatorNode::CreateOscillatorNode(
          base_audio_context_ptr_->GetLock(),
          OscillatorNode::GetDefaultOptions(),
          base_audio_context_ptr_->sample_rate());
  Napi::Object js_oscillator_node =
      JSOscillatorNode::FindClass<JSOscillatorNode>()
          .NewWithArgs<JSOscillatorNode>({info.This()}, oscillator_node_ptr);

  return js_oscillator_node;
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