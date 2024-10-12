#include "BaseAudioContext.hpp"
#include "AudioDestinationNode.hpp"
#include "AudioEngine.hpp"
#include "timer.hpp"

namespace js_audio {
BaseAudioContext::BaseAudioContext(const std::tuple<uint32_t, float> &params)
    : BaseAudioContext(std::get<0>(params), std::get<1>(params)) {}

// TODO: we now have 2 types of AudioContext, but we do not use AudioEngine  in
// OfflineAudioContext. So we need to add sampleRate, numberOfChannels and
// sampleRate to BaseAudioContext constructor and relative properties.
BaseAudioContext::BaseAudioContext(const uint32_t &number_of_channels,
                                   const float &sample_rate)
    : audio_context_lock_(std::make_shared<std::mutex>()),
      sample_rate_(sample_rate), number_of_channels_(number_of_channels),
      audio_destination_node_ptr_(std::make_shared<AudioDestinationNode>(
          number_of_channels, audio_context_lock_)),
      construct_microsecond_time_(
          GetCurrentSystemTime<double>(TimeUnit::kMicorsecond)) {}

std::shared_ptr<AudioDestinationNode>
BaseAudioContext::audio_destination_node_ptr() {
  return audio_destination_node_ptr_;
}

float BaseAudioContext::sample_rate() const { return sample_rate_; }
uint32_t BaseAudioContext::number_of_channels() const {
  return number_of_channels_;
}

double BaseAudioContext::GetCurrentTime() {
  double micro_diff = GetCurrentSystemTime<double>(TimeUnit::kMicorsecond) -
                      construct_microsecond_time_;
  return ConvertFromLeftTimeUnitToRight<TimeUnit::kMicorsecond,
                                        TimeUnit::kSecond>(micro_diff);
}

std::shared_ptr<std::mutex> BaseAudioContext::GetLock() const {
  return audio_context_lock_;
}

void BaseAudioContext::ProduceSamples(size_t sample_size,
                                      std::vector<SLint16> &output) {
  std::lock_guard<std::mutex> guard(*audio_context_lock_);
  audio_destination_node_ptr_->ProduceSamples(sample_size, output);
}

} // namespace js_audio
