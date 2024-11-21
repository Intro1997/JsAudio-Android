#include "BaseAudioContext.hpp"
#include "AudioDestinationNode.hpp"
#include "AudioEngine.hpp"
#include "logger.hpp"

namespace js_audio {
BaseAudioContext::BaseAudioContext(
    const std::tuple<ContextType, uint32_t, float> &params)
    : BaseAudioContext(std::get<0>(params), std::get<1>(params),
                       std::get<2>(params)) {}

// TODO: we now have 2 types of AudioContext, but we do not use AudioEngine  in
// OfflineAudioContext. So we need to add sampleRate, numberOfChannels and
// sampleRate to BaseAudioContext constructor and relative properties.
BaseAudioContext::BaseAudioContext(const ContextType &type,
                                   const uint32_t &number_of_channels,
                                   const float &sample_rate)
    : audio_context_lock_ref_(std::make_shared<std::mutex>()), current_time_(0),
      sample_rate_(sample_rate), number_of_channels_(number_of_channels),
      context_type_(type),
      audio_destination_node_ref_(std::make_shared<AudioDestinationNode>(
          number_of_channels, audio_context_lock_ref_)) {}

bool BaseAudioContext::IsOnlineContext() const {
  return context_type_ == ContextType::kOnline;
}

std::shared_ptr<AudioDestinationNode>
BaseAudioContext::audio_destination_node_ref() {
  return audio_destination_node_ref_;
}

void BaseAudioContext::UpdateCurrentTime(const size_t &sample_size) {}

float BaseAudioContext::sample_rate() const { return sample_rate_; }

uint32_t BaseAudioContext::number_of_channels() const {
  return number_of_channels_;
}

double BaseAudioContext::current_time() { return 0; }

double BaseAudioContext::GetCurrentTime() { return 0; }

std::shared_ptr<std::mutex> BaseAudioContext::GetLock() const {
  return audio_context_lock_ref_;
}

void BaseAudioContext::ProduceSamples(const size_t &sample_size,
                                      std::vector<std::vector<float>> &output) {
  std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
  audio_destination_node_ref_->ProduceSamples(sample_size, output);
}

} // namespace js_audio
