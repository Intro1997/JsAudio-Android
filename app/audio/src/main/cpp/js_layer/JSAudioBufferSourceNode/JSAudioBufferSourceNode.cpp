#include "JSAudioBufferSourceNode.hpp"
#include "AudioBufferSourceNode.hpp"
#include "JSAudioBuffer.hpp"
#include "JSAudioParam.hpp"
#include "JSBaseAudioContext.hpp"
#include <cmath>
#include <memory>

using AudioBuffer = js_audio::AudioBuffer;
using AudioBufferSourceNode = js_audio::AudioBufferSourceNode;
using AudioBufferSourceOptions =
    js_audio::AudioBufferSourceNode::AudioBufferSourceOptions;
using JSAudioBuffer = js_audio::JSAudioBuffer;
using JSBaseAudioContext = js_audio::JSBaseAudioContext;

static std::shared_ptr<AudioBufferSourceNode> GetAudioBufferSourceNodeRef(
    const Napi_IH::IHCallbackInfo &info,
    std::shared_ptr<AudioBufferSourceNode> audio_buffer_source_node_ref);

static bool ExtractOptionsFromInfo(const Napi_IH::IHCallbackInfo &info,
                                   AudioBufferSourceOptions &options,
                                   Napi::Error &napi_error);

static bool GetOptionsBuffer(Napi::Env env, const Napi::Object &napi_options,
                             std::shared_ptr<AudioBuffer> &buffer,
                             Napi::Error &napi_error);

static bool GetOptionsDetune(Napi::Env env, const Napi::Object &napi_options,
                             float &detune, Napi::Error &napi_error);

static bool GetOptionsLoop(Napi::Env env, const Napi::Object &napi_options,
                           bool &loop, Napi::Error &napi_error);

static bool GetOptionsLoopEnd(Napi::Env env, const Napi::Object &napi_options,
                              double &loop_end, Napi::Error &napi_error);

static bool GetOptionsLoopStart(Napi::Env env, const Napi::Object &napi_options,
                                double &loop_start, Napi::Error &napi_error);

static bool GetOptionsPlaybackRate(Napi::Env env,
                                   const Napi::Object &napi_options,
                                   float &playback_rate,
                                   Napi::Error &napi_error);

namespace js_audio {
void JSAudioBufferSourceNode::Init(Napi::Env env, Napi::Object exports) {
  DefineClass<JSAudioBufferSourceNode, JSAudioScheduledSourceNode>(
      env, "AudioBufferSourceNode",
      {InstanceAccessor<JSAudioBufferSourceNode,
                        &JSAudioBufferSourceNode::getBuffer,
                        &JSAudioBufferSourceNode::setBuffer>("buffer"),
       InstanceAccessor<JSAudioBufferSourceNode,
                        &JSAudioBufferSourceNode::getDetune>("detune"),
       InstanceAccessor<JSAudioBufferSourceNode,
                        &JSAudioBufferSourceNode::getLoop,
                        &JSAudioBufferSourceNode::setLoop>("loop"),
       InstanceAccessor<JSAudioBufferSourceNode,
                        &JSAudioBufferSourceNode::getLoopEnd,
                        &JSAudioBufferSourceNode::setLoopEnd>("loopEnd"),
       InstanceAccessor<JSAudioBufferSourceNode,
                        &JSAudioBufferSourceNode::getLoopStart,
                        &JSAudioBufferSourceNode::setLoopStart>("loopStart"),
       InstanceAccessor<JSAudioBufferSourceNode,
                        &JSAudioBufferSourceNode::getPlaybackRate>(
           "playbackRate"),
       InstanceMethod<JSAudioBufferSourceNode, &JSAudioBufferSourceNode::start>(
           "start")});
}

JSAudioBufferSourceNode::JSAudioBufferSourceNode(
    const Napi_IH::IHCallbackInfo &info,
    std::shared_ptr<AudioBufferSourceNode> audio_buffer_source_node_ref)
    : JSAudioScheduledSourceNode(info, GetAudioBufferSourceNodeRef(
                                           info, audio_buffer_source_node_ref)),
      has_set_buffer_(false) {
  audio_buffer_source_node_ptr_ =
      std::static_pointer_cast<AudioBufferSourceNode>(audio_node_ref_);

  auto ref = audio_buffer_source_node_ptr_.lock();
  if (ref && ref->audio_buffer_ref()) {
    Napi::Object napi_audio_buffer_obj =
        info[1].ToObject().Get("buffer").ToObject();
    napi_audio_buffer_ref_ = Napi::Persistent(napi_audio_buffer_obj);
  }
}

Napi::Value JSAudioBufferSourceNode::getBuffer(const Napi::CallbackInfo &info) {
  if (napi_audio_buffer_ref_.IsEmpty()) {
    return info.Env().Null();
  }
  return napi_audio_buffer_ref_.Value();
}

void JSAudioBufferSourceNode::setBuffer(const Napi::CallbackInfo &info,
                                        const Napi::Value &value) {
  if (value.IsNull() || value.IsUndefined()) {
    napi_audio_buffer_ref_.Reset();
    if (auto ref = audio_buffer_source_node_ptr_.lock()) {
      ref->set_audio_buffer_ref(nullptr);
    }
    return;
  }

  if (!napi_audio_buffer_ref_.IsEmpty() || has_set_buffer_) {
    throw Napi::Error::New(
        info.Env(),
        "Failed to set the 'buffer' property on 'AudioBufferSourceNode': "
        "Cannot set buffer to non-null after it has been already been set to a "
        "non-null buffer.\n");
  }
  if (!value.IsObject() ||
      !Napi_IH::VerifyExactInstanceType(value.ToObject(), "AudioBuffer")) {
    throw Napi::TypeError::New(
        info.Env(),
        "Failed to set the 'buffer' property on 'AudioBufferSourceNode': "
        "Failed to convert value to 'AudioBuffer'.\n");
  }

  Napi::Object napi_audio_buffer_obj = value.ToObject();
  auto *js_audio_buffer_ptr = UnWrap<JSAudioBuffer>(napi_audio_buffer_obj);
  if (auto audio_buffer_source_node_ref =
          audio_buffer_source_node_ptr_.lock()) {
    audio_buffer_source_node_ref->set_audio_buffer_ref(
        js_audio_buffer_ptr->GetInnerAudioBuffer());
  }
  napi_audio_buffer_ref_ = Napi::Persistent(napi_audio_buffer_obj);
  has_set_buffer_ = true;
}

// readonly detune: AudioParam;
Napi::Value JSAudioBufferSourceNode::getDetune(const Napi::CallbackInfo &info) {
  if (auto audio_buffer_source_node_ref =
          audio_buffer_source_node_ptr_.lock()) {
    std::shared_ptr<AudioParam> detune_ref =
        audio_buffer_source_node_ref->detune_ref();
    return FindClass<JSAudioParam>().NewWithArgs<JSAudioParam>({}, detune_ref);
  }
  return info.Env().Undefined();
}

// loop: boolean;
Napi::Value JSAudioBufferSourceNode::getLoop(const Napi::CallbackInfo &info) {
  if (auto audio_buffer_source_node_ref =
          audio_buffer_source_node_ptr_.lock()) {
    return Napi::Boolean::New(info.Env(), audio_buffer_source_node_ref->loop());
  }
  return info.Env().Undefined();
}

void JSAudioBufferSourceNode::setLoop(const Napi::CallbackInfo &info,
                                      const Napi::Value &value) {
  if (auto audio_buffer_source_node_ref =
          audio_buffer_source_node_ptr_.lock()) {
    audio_buffer_source_node_ref->set_loop(value.ToBoolean());
  }
}

// loopEnd: number;
Napi::Value
JSAudioBufferSourceNode::getLoopEnd(const Napi::CallbackInfo &info) {
  if (auto audio_buffer_source_node_ref =
          audio_buffer_source_node_ptr_.lock()) {
    return Napi::Number::New(info.Env(), audio_buffer_source_node_ref->loop());
  }
  return info.Env().Undefined();
}

void JSAudioBufferSourceNode::setLoopEnd(const Napi::CallbackInfo &info,
                                         const Napi::Value &value) {
  double maybe_loop_end = value.ToNumber();
  if (std::isnan(maybe_loop_end)) {
    throw Napi::TypeError::New(
        info.Env(),
        "Failed to set the 'loopEnd' property on 'AudioBufferSourceNode': The "
        "provided double value is non-finite.\n");
  }
  if (auto audio_buffer_source_node_ref =
          audio_buffer_source_node_ptr_.lock()) {
    audio_buffer_source_node_ref->set_loop_end(maybe_loop_end);
  }
}

// loopStart: number;
Napi::Value
JSAudioBufferSourceNode::getLoopStart(const Napi::CallbackInfo &info) {
  if (auto audio_buffer_source_node_ref =
          audio_buffer_source_node_ptr_.lock()) {
    return Napi::Number::New(info.Env(),
                             audio_buffer_source_node_ref->loop_start());
  }
  return info.Env().Undefined();
}

void JSAudioBufferSourceNode::setLoopStart(const Napi::CallbackInfo &info,
                                           const Napi::Value &value) {
  double maybe_loop_start = value.ToNumber();
  if (std::isnan(maybe_loop_start)) {
    throw Napi::TypeError::New(
        info.Env(),
        "Failed to set the 'loopStart' property on 'AudioBufferSourceNode': "
        "The provided double value is non-finite.\n");
  }
  if (auto audio_buffer_source_node_ref =
          audio_buffer_source_node_ptr_.lock()) {
    audio_buffer_source_node_ref->set_loop_start(maybe_loop_start);
  }
}

Napi::Value
JSAudioBufferSourceNode::getPlaybackRate(const Napi::CallbackInfo &info) {
  if (auto audio_buffer_source_node_ref =
          audio_buffer_source_node_ptr_.lock()) {
    std::shared_ptr<AudioParam> playback_rate_ref =
        audio_buffer_source_node_ref->playback_rate_ref();
    return FindClass<JSAudioParam>().NewWithArgs<JSAudioParam>(
        {}, playback_rate_ref);
  }
  return info.Env().Undefined();
}

Napi::Value JSAudioBufferSourceNode::start(const Napi::CallbackInfo &info) {
  if (audio_node_ref_ &&
      std::static_pointer_cast<AudioBufferSourceNode>(audio_node_ref_)
          ->has_started()) {
    throw Napi::Error::New(
        info.Env(), "Failed to execute 'start' on 'AudioBufferSourceNode': "
                    "cannot call start more than once.\n");
  }
  double when = 0;
  double offset = 0;
  double duration = -1;

  if (info.Length() > 0) {
    when = info[0].ToNumber();
    Napi::Error napi_error;
    if (!IsWhenValid(when, "AudioBufferSourceNode", "start", info.Env(),
                     napi_error)) {
      throw napi_error;
    }
  }
  if (info.Length() > 1) {
    double maybe_offset = info[1].ToNumber();
    if (std::isnan(maybe_offset)) {
      throw Napi::TypeError::New(
          info.Env(), "Failed to execute 'start' on 'AudioBufferSourceNode': "
                      "The provided float value is non-finite.\n");
    }
    if (maybe_offset < 0) {
      throw Napi_IH::RangeError::New(
          info.Env(),
          "Failed to execute 'start' on 'AudioBufferSourceNode': The offset "
          "provided (%f) is less than the minimum bound (0).\n",
          maybe_offset);
    }
    offset = maybe_offset;
  }
  if (info.Length() > 2) {
    double maybe_duration = info[2].ToNumber();
    if (std::isnan(maybe_duration)) {
      throw Napi::TypeError::New(
          info.Env(), "Failed to execute 'start' on 'AudioBufferSourceNode': "
                      "The provided double value is non-finite.\n");
    }
    if (maybe_duration < 0) {
      throw Napi_IH::RangeError::New(
          info.Env(),
          "Failed to execute 'start' on 'AudioBufferSourceNode': The duration "
          "provided (%f) is less than the minimum bound (0).\n",
          maybe_duration);
    }
    duration = maybe_duration;
  }

  if (auto audio_buffer_source_node_ref =
          audio_buffer_source_node_ptr_.lock()) {
    audio_buffer_source_node_ref->Start(when, offset, duration);
  }

  return info.Env().Undefined();
}

} // namespace js_audio

static std::shared_ptr<AudioBufferSourceNode> GetAudioBufferSourceNodeRef(
    const Napi_IH::IHCallbackInfo &info,
    std::shared_ptr<AudioBufferSourceNode> audio_buffer_source_node_ref) {
  if (audio_buffer_source_node_ref) {
    return audio_buffer_source_node_ref;
  }
  if (info.Length() < 1) {
    throw Napi_IH::TypeError::New(
        info.Env(), "Failed to construct 'AudioBufferSourceNode': 1 argument "
                    "required, but only 0 present.");
  }
  if (!info[0].IsObject()) {
    throw Napi::TypeError::New(
        info.Env(), "Failed to construct 'AudioBufferSourceNode': parameter 1 "
                    "is not of type 'BaseAudioContext'.\n");
  }
  if (!Napi_IH::VerifyInstanceOf<JSBaseAudioContext>(info[0].ToObject())) {
    throw Napi::TypeError::New(
        info.Env(), "Failed to construct 'AudioBufferSourceNode': parameter 1 "
                    "is not of type 'BaseAudioContext'.\n");
  }
  auto *js_base_audio_context_ptr =
      Napi_IH::IHObjectWrap::UnWrap<JSBaseAudioContext>(info[0].ToObject());
  auto options = AudioBufferSourceNode::GetDefaultOptions();
  Napi::Error napi_error;
  if (!ExtractOptionsFromInfo(info, options, napi_error)) {
    throw napi_error;
  }

  return AudioBufferSourceNode::CreateAudioBufferSourceNode(
      options, js_base_audio_context_ptr->GetSampleRate(),
      js_base_audio_context_ptr->GetAudioContextLock());
}

static bool ExtractOptionsFromInfo(const Napi_IH::IHCallbackInfo &info,
                                   AudioBufferSourceOptions &options,
                                   Napi::Error &napi_error) {
  if (info.Length() < 2 || info[1].IsNull() || info[1].IsUndefined()) {
    return true;
  }
  if (!info[1].IsObject()) {
    napi_error = Napi::TypeError::New(
        info.Env(), "Failed to construct 'AudioBufferSourceNode': The provided "
                    "value is not of type 'AudioBufferSourceOptions'.\n");
    return false;
  }
  Napi::Object napi_options = info[1].ToObject();
  Napi::Env env = info.Env();

  if (!GetOptionsBuffer(env, napi_options, options.buffer_ref, napi_error)) {
    return false;
  }
  if (!GetOptionsDetune(env, napi_options, options.detune, napi_error)) {
    return false;
  }
  if (!GetOptionsLoop(env, napi_options, options.loop, napi_error)) {
    return false;
  }
  if (!GetOptionsLoopEnd(env, napi_options, options.loop_end, napi_error)) {
    return false;
  }
  if (!GetOptionsLoopStart(env, napi_options, options.loop_start, napi_error)) {
    return false;
  }
  if (!GetOptionsPlaybackRate(env, napi_options, options.playback_rate,
                              napi_error)) {
    return false;
  }
  return true;
}

static bool GetOptionsBuffer(Napi::Env env, const Napi::Object &napi_options,
                             std::shared_ptr<AudioBuffer> &buffer,
                             Napi::Error &napi_error) {
  if (!napi_options.IsObject()) {
    napi_error = Napi::TypeError::New(
        env, "Failed to construct 'AudioBufferSourceNode': The provided value "
             "is not of type 'AudioBufferSourceOptions'.\n");
    return false;
  }
  if (napi_options.Has("buffer")) {
    if (napi_options["buffer"].IsNull() ||
        napi_options["buffer"].IsUndefined()) {
      return true;
    }

    if (!napi_options["buffer"].IsObject() ||
        !Napi_IH::VerifyExactInstanceType(napi_options["buffer"].ToObject(),
                                          "AudioBuffer")) {
      napi_error = Napi::TypeError::New(
          env, "Failed to construct 'AudioBufferSourceNode': Failed to read "
               "the 'buffer' property from 'AudioBufferSourceOptions': Failed "
               "to convert value to 'AudioBuffer'.\n");
      return false;
    }
    const auto *js_audio_buffer_ptr =
        Napi_IH::IHObjectWrap::UnWrap<JSAudioBuffer>(
            napi_options["buffer"].ToObject());
    buffer = js_audio_buffer_ptr->GetInnerAudioBuffer();
  }
  return true;
}

static bool GetOptionsDetune(Napi::Env env, const Napi::Object &napi_options,
                             float &detune, Napi::Error &napi_error) {
  if (!napi_options.IsObject()) {
    napi_error = Napi::TypeError::New(
        env, "Failed to construct 'AudioBufferSourceNode': The provided value "
             "is not of type 'AudioBufferSourceOptions'.\n");
    return false;
  }
  if (napi_options.Has("detune")) {
    float maybe_detune = napi_options["detune"].ToNumber();
    if (std::isnan(maybe_detune)) {
      napi_error = Napi::TypeError::New(
          env, "Failed to construct 'AudioBufferSourceNode': Failed to read "
               "the 'detune' property from 'AudioBufferSourceOptions': The "
               "provided float value is non-finite.\n");
      return false;
    }
    detune = maybe_detune;
  }
  return true;
}

static bool GetOptionsLoop(Napi::Env env, const Napi::Object &napi_options,
                           bool &loop, Napi::Error &napi_error) {
  if (!napi_options.IsObject()) {
    napi_error = Napi::TypeError::New(
        env, "Failed to construct 'AudioBufferSourceNode': The provided value "
             "is not of type 'AudioBufferSourceOptions'.\n");
    return false;
  }
  if (napi_options.Has("loop")) {
    bool maybe_loop = napi_options["loop"].ToBoolean();
    loop = maybe_loop;
  }
  return true;
}

static bool GetOptionsLoopEnd(Napi::Env env, const Napi::Object &napi_options,
                              double &loop_end, Napi::Error &napi_error) {
  if (!napi_options.IsObject()) {
    napi_error = Napi::TypeError::New(
        env, "Failed to construct 'AudioBufferSourceNode': The provided value "
             "is not of type 'AudioBufferSourceOptions'.\n");
    return false;
  }
  if (napi_options.Has("loopEnd")) {
    double maybe_loop_end = napi_options["loopEnd"].ToNumber();
    if (std::isnan(maybe_loop_end)) {
      napi_error = Napi::TypeError::New(
          env, "Failed to construct 'AudioBufferSourceNode': Failed to read "
               "the 'loopEnd' property from 'AudioBufferSourceOptions': The "
               "provided float value is non-finite.\n");
      return false;
    }
    loop_end = maybe_loop_end;
  }
  return true;
}

static bool GetOptionsLoopStart(Napi::Env env, const Napi::Object &napi_options,
                                double &loop_start, Napi::Error &napi_error) {
  if (!napi_options.IsObject()) {
    napi_error = Napi::TypeError::New(
        env, "Failed to construct 'AudioBufferSourceNode': The provided value "
             "is not of type 'AudioBufferSourceOptions'.\n");
    return false;
  }
  if (napi_options.Has("loopStart")) {
    double maybe_loop_start = napi_options["loopStart"].ToNumber();
    if (std::isnan(maybe_loop_start)) {
      napi_error = Napi::TypeError::New(
          env, "Failed to construct 'AudioBufferSourceNode': Failed to read "
               "the 'loopStart' property from 'AudioBufferSourceOptions': The "
               "provided float value is non-finite.\n");
      return false;
    }
    loop_start = maybe_loop_start;
  }
  return true;
}

static bool GetOptionsPlaybackRate(Napi::Env env,
                                   const Napi::Object &napi_options,
                                   float &playback_rate,
                                   Napi::Error &napi_error) {
  if (!napi_options.IsObject()) {
    napi_error = Napi::TypeError::New(
        env, "Failed to construct 'AudioBufferSourceNode': The provided value "
             "is not of type 'AudioBufferSourceOptions'.\n");
    return false;
  }
  if (napi_options.Has("playbackRate")) {
    float maybe_playback_rate = napi_options["playbackRate"].ToNumber();
    if (std::isnan(maybe_playback_rate)) {
      napi_error = Napi::TypeError::New(
          env, "Failed to construct 'AudioBufferSourceNode': Failed to read "
               "the 'playbackRate' property from 'AudioBufferSourceOptions': "
               "The provided float value is non-finite.\n");
      return false;
    }
    playback_rate = maybe_playback_rate;
  }
  return true;
}
