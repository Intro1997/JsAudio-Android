#include "JSBaseAudioContext.hpp"
#include "AudioEngine.hpp"
#include "DelayNode.hpp"
#include "GainNode.hpp"
#include "JSAudioBuffer.hpp"
#include "JSAudioDestinationNode.hpp"
#include "JSDelayNode.hpp"
#include "JSGainNode.hpp"
#include "JSOscillatorNode.hpp"
#include "OscillatorNode.hpp"

namespace js_audio {

JSBaseAudioContext::JSBaseAudioContext(
    const Napi_IH::IHCallbackInfo &info,
    std::shared_ptr<BaseAudioContext> base_audio_context_ref)
    : Napi_IH::IHObjectWrap(info) {
  if (base_audio_context_ref) {
    base_audio_context_ref_ = base_audio_context_ref;
  } else {
    // Shouldn't get in here when use inheritance
    base_audio_context_ref_ = std::make_shared<BaseAudioContext>();
  }

  Napi::Object js_destination_node =
      JSAudioDestinationNode::FindClass<JSAudioDestinationNode>()
          .NewWithArgs<JSAudioDestinationNode>(
              {info.This()},
              base_audio_context_ref_->audio_destination_node_ref());

  napi_destination_node_ref_ = Napi::Persistent(js_destination_node);

  bool is_add_to_audio_player = false;
  auto audio_engine_ptr = AudioEngine::Get().lock();
  if (audio_engine_ptr && base_audio_context_ref_->IsOnlineContext()) {
    if (auto audio_player_ptr =
            audio_engine_ptr
                ->GetAudioPlayer(AudioPlayerType::kBufferQueuePlayer)
                .lock()) {
      is_add_to_audio_player =
          audio_player_ptr->AddBaseAudioContext(base_audio_context_ref_);
    }
  }

  if (!is_add_to_audio_player && base_audio_context_ref_->IsOnlineContext()) {
    // we do not need to reserve cpp pointer
    // when add to audio player failed
    base_audio_context_ref_.reset();
    napi_destination_node_ref_.Reset();
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
          InstanceMethod<JSBaseAudioContext, &JSBaseAudioContext::createGain>(
              "createGain"),
          InstanceMethod<JSBaseAudioContext, &JSBaseAudioContext::createDelay>(
              "createDelay"),
      },
      Napi_IH::ClassVisibility::kHideConstructor);
}

Napi::Value
JSBaseAudioContext::getDestinationNode(const Napi::CallbackInfo &info) {
  return napi_destination_node_ref_.Value();
}

Napi::Value JSBaseAudioContext::getSampleRate(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  return Napi::Number::From(env, base_audio_context_ref_->sample_rate());
}

Napi::Value JSBaseAudioContext::getCurrentTime(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  return Napi::Number::From(env, base_audio_context_ref_->GetCurrentTime());
}

std::shared_ptr<std::mutex> JSBaseAudioContext::GetAudioContextLock() const {
  return base_audio_context_ref_->GetLock();
}

std::shared_ptr<BaseAudioContext> JSBaseAudioContext::GetAudioContext() const {
  return base_audio_context_ref_;
}

float JSBaseAudioContext::GetSampleRate() const {
  return base_audio_context_ref_->sample_rate();
}

Napi::Value
JSBaseAudioContext::createOscillator(const Napi::CallbackInfo &info) {
  std::shared_ptr<OscillatorNode> oscillator_node_ref =
      OscillatorNode::CreateOscillatorNode(GetSampleRate(),
                                           GetAudioContextLock());
  Napi::Object js_oscillator_node =
      JSOscillatorNode::FindClass<JSOscillatorNode>()
          .NewWithArgs<JSOscillatorNode>({info.This()}, oscillator_node_ref);

  return js_oscillator_node;
}

Napi::Value JSBaseAudioContext::createBuffer(const Napi::CallbackInfo &info) {
  if (info.Length() < 3) {
    LOGE("Create JSAudioBuffer failed! Need 3 parameters, but get %zu.\n",
         info.Length());
    return info.Env().Undefined();
  }
  // TODO: maybe create shared pointer of AudioBuffer is better?
  if (info[0].IsNumber() && info[1].IsNumber() && info[2].IsNumber()) {
    Napi::Object options = Napi::Object::New(info.Env());
    options.Set("numberOfChannels", info[0]);
    options.Set("length", info[1]);
    options.Set("sampleRate", info[2]);
    Napi::Object js_audio_buffer =
        JSAudioBuffer::FindClass<JSAudioBuffer>().NewWithArgs<JSAudioBuffer>(
            {options});

    return js_audio_buffer;
  } else {
    LOGE("Create JSAudioBuffer failed! Need 3 number parameters.\n");
  }
  return info.Env().Undefined();
}

Napi::Value JSBaseAudioContext::createGain(const Napi::CallbackInfo &info) {
  std::shared_ptr<GainNode> gain_node_ref =
      GainNode::CreateGainNode(GetAudioContextLock());
  return FindClass<JSGainNode>().NewWithArgs<JSGainNode>({info.This()},
                                                         gain_node_ref);
}

Napi::Value JSBaseAudioContext::createDelay(const Napi::CallbackInfo &info) {
  auto options = DelayNode::GetDefaultOptions();
  if (info.Length() > 0) {
    float max_delay_time = info[0].ToNumber();
    if (std::isnan(max_delay_time)) {
      throw Napi::TypeError::New(
          info.Env(), "Failed to execute 'createDelay' on 'BaseAudioContext': "
                      "The provided double value is non-finite.\n");
    } else if (!DelayNode::IsValidMaxDelayTime(max_delay_time)) {
      throw Napi_IH::RangeError::New(
          info.Env(),
          "Failed to execute 'createDelay' on 'BaseAudioContext': The max "
          "delay time provided (%f) is outside the range (%f, %f).\n",
          max_delay_time, DelayNode::kMinOfMaxDelayTime,
          DelayNode::kMaxOfMaxDelayTime);
    }
    options.max_delay_time = max_delay_time;
  }
  std::shared_ptr<DelayNode> delay_node_ref = DelayNode::CreateDelayNode(
      options, GetSampleRate(), GetAudioContextLock());
  return FindClass<JSDelayNode>().NewWithArgs<JSDelayNode>({info.This()},
                                                           delay_node_ref);
}
} // namespace js_audio