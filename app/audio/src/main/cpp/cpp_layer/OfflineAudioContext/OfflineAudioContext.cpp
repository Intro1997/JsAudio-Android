#include "OfflineAudioContext.hpp"
#include "logger.hpp"

#include <chrono>
#include <thread>

// refer to
// https://github.com/web-platform-tests/wpt/blob/master/webaudio/the-audio-api/the-offlineaudiocontext-interface/current-time-block-size.html
static const size_t kCurrentTimeStep = 128;
namespace js_audio {

OfflineAudioContext::OfflineAudioContext(const uint32_t &number_of_channels,
                                         const uint32_t &length,
                                         const float &sample_rate)
    : BaseAudioContext(ContextType::kOffline, number_of_channels, sample_rate),
      current_time_(0), length_(length) {
  number_of_channels_ = number_of_channels;
  sample_rate_ = sample_rate;
  set_render_state(RenderState::kFinish);
}

// TODO: alias a type ot cb
bool OfflineAudioContext::StartRendering(
    const std::function<void(std::shared_ptr<AudioBuffer>)> &cb_ref) {
  if (render_state() != RenderState::kFinish) {
    return false;
  }
  std::thread offline_rendering_thread([=]() { this->InnerRendering(cb_ref); });
  offline_rendering_thread.detach();
  return true;
}

double OfflineAudioContext::GetCurrentTime() {
  std::lock_guard<std::mutex> guard(current_time_lock_);
  return current_time_;
}

const uint32_t &OfflineAudioContext::length() const { return length_; }

void OfflineAudioContext::UpdateCurrentTime() {
  uint32_t step_cnt = length_ / kCurrentTimeStep;
  step_cnt += (length_ % kCurrentTimeStep == 0) ? 0 : 1;
  std::lock_guard<std::mutex> gurard(current_time_lock_);
  current_time_ = (double)(step_cnt * kCurrentTimeStep) / (double)sample_rate_;
}

void OfflineAudioContext::InitOutputArray(
    std::vector<std::vector<float>> &output) {
  output.resize(number_of_channels_);
  for (auto &channel : output) {
    channel.resize(length_);
  }
}

// TODO: alias a type ot cb
void OfflineAudioContext::InnerRendering(
    const std::function<void(std::shared_ptr<AudioBuffer>)> &cb_ref) {
  if (render_state() != RenderState::kFinish) {
    return;
  }

  std::vector<std::vector<float>> output;
  InitOutputArray(output);

  set_render_state(RenderState::kRunning);
  ProduceSamples(length_, output);
  UpdateCurrentTime();
  set_render_state(RenderState::kFinish);

  if (cb_ref) {
    auto rendered_buffer_ptr = std::make_shared<AudioBuffer>(std::move(output));
    cb_ref(rendered_buffer_ptr);
  }
}

OfflineAudioContext::RenderState OfflineAudioContext::render_state() const {
  std::lock_guard<std::mutex> guard(render_state_lock_);
  return render_state_;
}

void OfflineAudioContext::set_render_state(RenderState state) {
  std::lock_guard<std::mutex> guard(render_state_lock_);
  render_state_ = state;
}

} // namespace js_audio
