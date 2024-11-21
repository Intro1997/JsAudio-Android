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
  stop_time_ = INFINITY;
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

void AudioScheduledSourceNode::Start(const double &when) {
  if (has_started()) {
    LOGE("Start AudioScheduledSourceNode failed! Node can only start once!\n");
    return;
  }
  if (when < 0 || std::isnan(when)) {
    LOGE("invalid when time %f\n", when);
    return;
  }

  set_has_started(true);
  set_start_time(when);
  set_state(State::Start);
}

void AudioScheduledSourceNode::Stop(const double &when) {
  if (state() == State::Stop) {
    return;
  }
  if (when < 0 || std::isnan(when)) {
    LOGE("invalid when time %f\n", when);
    return;
  }
  set_stop_time(when);
  set_state(State::Start);
}

bool AudioScheduledSourceNode::has_started() const { return has_started_; }

void AudioScheduledSourceNode::set_has_started(const bool &value) {
  has_started_ = value;
}

double AudioScheduledSourceNode::start_time() const {
  std::lock_guard<std::mutex> guard(start_time_lock_);
  return start_time_;
}
void AudioScheduledSourceNode::set_start_time(const double &time) {
  std::lock_guard<std::mutex> guard(start_time_lock_);
  start_time_ = time;
}
double AudioScheduledSourceNode::stop_time() const {
  std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
  return stop_time_;
}
void AudioScheduledSourceNode::set_stop_time(const double &time) {
  std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
  stop_time_ = time;
}

AudioScheduledSourceNode::State AudioScheduledSourceNode::state() const {
  std::lock_guard<std::mutex> guard(state_lock_);
  return *state_ref_;
}

void AudioScheduledSourceNode::set_state(const State &state) {
  std::lock_guard<std::mutex> guard(state_lock_);
  *state_ref_ = state;
}

}; // namespace js_audio