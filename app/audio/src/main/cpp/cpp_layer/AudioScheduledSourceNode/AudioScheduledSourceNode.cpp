#include "AudioScheduledSourceNode.hpp"
#include "BaseAudioContext.hpp"
#include "logger.hpp"

#include <chrono>
#include <cmath>
#include <thread>
namespace js_audio {
AudioScheduledSourceNode::AudioScheduledSourceNode(
    const uint32_t &number_of_inputs, const uint32_t &number_of_outputs,
    const uint32_t &channel_count,
    const AudioNode::ChannelCountMode &channel_count_mode,
    const AudioNode::ChannelInterpretation &channel_interpretation,
    std::shared_ptr<std::mutex> audio_context_lock_ref)
    : AudioNode(number_of_inputs, number_of_outputs, channel_count,
                channel_count_mode, channel_interpretation,
                audio_context_lock_ref) {
  state_ref_ = std::make_shared<State>(State::Stop);
  has_started_ = false;
}

void AudioScheduledSourceNode::ConnectTo(
    std::shared_ptr<AudioNode> dst_audio_node_ref) {
  if (IsSelfPtr(dst_audio_node_ref)) {
    LOGE("Error! Cannot connect to self!\n");
    return;
  }

  std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
  dst_audio_node_ptr_ = dst_audio_node_ref;
}

void AudioScheduledSourceNode::BeConnectedTo(
    std::shared_ptr<AudioNode> src_audio_node_ref) {
  LOGE("Error! Should not get in here!\n");
}

void AudioScheduledSourceNode::Disconnect() {
  if (auto dst_audio_node_ref = dst_audio_node_ptr_.lock()) {
    dst_audio_node_ref->BeDisconnected(*this);
  }
  std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
  dst_audio_node_ptr_.reset();
}

void AudioScheduledSourceNode::BeDisconnected(const AudioNode &audio_node) {
  LOGE("Error! Should not get in here!\n");
}

void AudioScheduledSourceNode::Start(const float &when) {
  if (has_started_) {
    LOGE("Start AudioScheduledSourceNode failed! Node can only start once!\n");
    return;
  }
  if (when < 0 || std::isnan(when)) {
    LOGE("invalid when time %f\n", when);
    return;
  }

  has_started_ = true;

  if (std::shared_ptr<BaseAudioContext> base_audio_context_ref =
          base_audio_context_ptr_.lock()) {
    float wait_time = when - base_audio_context_ref->GetCurrentTime();
    if (wait_time <= 0) {
      set_state(State::Start);
    } else {
      ScheduleStateChange(State::Start, wait_time);
    }
  } else {
    LOGE(
        "AudioScheduleSourceNode start failed! BaseAudioContext is invalid!\n");
  }
}

void AudioScheduledSourceNode::Stop(const float &when) {
  if (state() == State::Stop) {
    return;
  }
  if (when < 0 || std::isnan(when)) {
    LOGE("invalid when time %f\n", when);
    return;
  }

  if (std::shared_ptr<BaseAudioContext> base_audio_context_ref =
          base_audio_context_ptr_.lock()) {
    float wait_time = when - base_audio_context_ref->GetCurrentTime();
    if (wait_time <= 0) {
      set_state(State::Stop);
    } else {
      ScheduleStateChange(State::Stop, wait_time);
    }
  } else {
    LOGE("AudioScheduleSourceNode stop failed! BaseAudioContext is invalid!\n");
  }
}

AudioScheduledSourceNode::State AudioScheduledSourceNode::state() const {
  std::lock_guard<std::mutex> guard(state_lock_);
  return *state_ref_;
}

void AudioScheduledSourceNode::set_state(const State &state) {
  std::lock_guard<std::mutex> guard(state_lock_);
  *state_ref_ = state;
}

void AudioScheduledSourceNode::ScheduleStateChange(const State &state,
                                                   const float &delay_time) {
  std::weak_ptr<State> state_ptr = state_ref_;
  std::thread t([=]() {
    std::this_thread::sleep_for(std::chrono::duration<float>(delay_time));
    if (auto state_ref = state_ptr.lock()) {
      *state_ref = state;
    }
  });
  t.detach();
}
}; // namespace js_audio