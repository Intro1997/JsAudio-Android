#include "OfflineAudioContext.hpp"
#include "logger.hpp"

#include "OfflineAudioContext.hpp"
#include "logger.hpp"
#include "timer.hpp"
#include <chrono>
#include <thread>

#include <chrono>
#include <thread>

// refer to
// https://github.com/web-platform-tests/wpt/blob/master/webaudio/the-audio-api/the-offlineaudiocontext-interface/current-time-block-size.html
static const size_t kCurrentTimeStep = 128;
static const size_t kMaxSampleTime = 8192;

static void AppendOutputToDst(std::vector<std::vector<float>> &dst,
                              const std::vector<std::vector<float>> &src,
                              const size_t &request_sample_size);

namespace js_audio {

OfflineAudioContext::OfflineAudioContext(const uint32_t &number_of_channels,
                                         const uint32_t &length,
                                         const float &sample_rate)
    : BaseAudioContext(ContextType::kOffline, number_of_channels, sample_rate),
      length_(length) {
  number_of_channels_ = number_of_channels;
  sample_rate_ = sample_rate;
  set_render_state(RenderState::kFinish);
}

// TODO: alias a type ot cb
bool OfflineAudioContext::StartRendering(
    const std::function<void(std::shared_ptr<AudioBuffer>)> &cb_ref,
    std::weak_ptr<BaseAudioContext> self_ptr) {
  if (render_state() != RenderState::kFinish) {
    return false;
  }
  std::thread offline_rendering_thread(
      [=]() { this->InnerRendering(cb_ref, self_ptr); });
  offline_rendering_thread.detach();
  return true;
}

double OfflineAudioContext::current_time() {
  std::lock_guard<std::mutex> guard(current_time_lock_);
  return current_time_;
}

double OfflineAudioContext::GetCurrentTime() {
  double current_time = OfflineAudioContext::current_time();
  size_t sample_size = current_time * sample_rate_ + 0.5;
  size_t step_cnt = sample_size % kCurrentTimeStep == 0 ? 0 : 1;
  step_cnt += sample_size / kCurrentTimeStep;
  return (double)(step_cnt * kCurrentTimeStep) / sample_rate_;
}

const uint32_t &OfflineAudioContext::length() const { return length_; }

void OfflineAudioContext::UpdateCurrentTime(const size_t &sample_size) {
  std::lock_guard<std::mutex> guard(current_time_lock_);
  current_time_ += (double)sample_size / (double)sample_rate_;
}

void OfflineAudioContext::InitOutputArray(
    std::vector<std::vector<float>> &output, const size_t size) {
  output.resize(number_of_channels_);
  for (auto &channel : output) {
    channel.resize(size);
  }
}

// TODO: alias a type ot cb
void OfflineAudioContext::InnerRendering(
    const std::function<void(std::shared_ptr<AudioBuffer>)> &cb_ref,
    std::weak_ptr<BaseAudioContext> self_ptr) {
  auto self_ref = self_ptr.lock();
  if (!self_ref) {
    LOGE("Invalid offline audio context\n");
    return;
  }
  if (render_state() != RenderState::kFinish) {
    return;
  }

  set_render_state(RenderState::kRunning);
  std::vector<std::vector<float>> final_output;
  InitOutputArray(final_output, 0);

  while (!final_output.empty() && final_output[0].size() < length_) {
    std::vector<std::vector<float>> output;
    const size_t total_request_size = length_ - final_output[0].size();
    const size_t current_request_size = kMaxSampleTime < total_request_size
                                            ? kMaxSampleTime
                                            : total_request_size;
    InitOutputArray(output, current_request_size);
    ProduceSamples(current_request_size, output);
    UpdateCurrentTime(current_request_size);
    AppendOutputToDst(final_output, output, current_request_size);
  }
  set_render_state(RenderState::kFinish);

  if (cb_ref) {
    auto rendered_buffer_ptr =
        std::make_shared<AudioBuffer>(std::move(final_output));
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

static void AppendOutputToDst(std::vector<std::vector<float>> &dst,
                              const std::vector<std::vector<float>> &src,
                              const size_t &request_sample_size) {
  const size_t actual_request_sample_size =
      std::min(kMaxSampleTime, request_sample_size);
  bool error = false;
  if (dst.size() != src.size()) {
    LOGE("Error: dst size is not equal to src size!\n");
    error = true;
  } else if (src.empty()) {
    LOGE("Error: src is empty!\n");
    error = true;
  }

  if (error) {
    for (auto &channel : dst) {
      std::fill(channel.begin(), channel.end(), 0);
    }
    return;
  }

  const size_t channel_size = dst.size();
  for (size_t c = 0; c < channel_size; c++) {
    std::vector<float> &dst_channel_ref = dst[c];
    const std::vector<float> &src_channel_ref = src[c];
    const size_t insert_sample_size =
        src_channel_ref.size() < actual_request_sample_size
            ? src_channel_ref.size()
            : actual_request_sample_size;

    dst_channel_ref.insert(dst_channel_ref.end(), src_channel_ref.begin(),
                           src_channel_ref.begin() + insert_sample_size);

    if (insert_sample_size < actual_request_sample_size) {
      std::vector<float> zero_vec(
          actual_request_sample_size - insert_sample_size, 0);
      dst_channel_ref.insert(dst_channel_ref.end(), zero_vec.begin(),
                             zero_vec.end());
    }
  }
}
