#include "OfflineAudioContext.hpp"
#include "logger.hpp"

#include <chrono>
#include <thread>

namespace js_audio {

OfflineAudioContext::OfflineAudioContext(const uint32_t &number_of_channels,
                                         const uint32_t &length,
                                         const float &sample_rate)
    : BaseAudioContext(number_of_channels, sample_rate), length_(length) {
  number_of_channels_ = number_of_channels;
  sample_rate_ = sample_rate_;
  set_render_state(RenderState::kFinish);
}

// TODO: alias a type ot cb
bool OfflineAudioContext::StartRendering(
    const std::function<void(std::shared_ptr<AudioBuffer>)> &cb) {
  if (render_state() != RenderState::kFinish) {
    return false;
  }
  std::thread offline_rendering_thread([=]() { this->InnerRendering(cb); });
  offline_rendering_thread.detach();
  return true;
}

const uint32_t &OfflineAudioContext::length() const { return length_; }

// TODO: alias a type ot cb
void OfflineAudioContext::InnerRendering(
    const std::function<void(std::shared_ptr<AudioBuffer>)> &cb) {
  if (render_state() != RenderState::kFinish) {
    return;
  }
  set_render_state(RenderState::kRunning);
  {
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(2s);
  }
  set_render_state(RenderState::kFinish);
  if (cb) {
    auto rendered_buffer_ptr = std::make_shared<AudioBuffer>(
        number_of_channels_, length_, sample_rate());
    float test_arr[10];
    for (int i = 0; i < 10; i++) {
      test_arr[i] = i;
    }
    rendered_buffer_ptr->CopyToChannel(test_arr, 10, 0, 0);
    for (int i = 0; i < 10; i++) {
      test_arr[i] = 10 - i;
    }
    rendered_buffer_ptr->CopyToChannel(test_arr, 10, 1, 0);
    cb(rendered_buffer_ptr);
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
