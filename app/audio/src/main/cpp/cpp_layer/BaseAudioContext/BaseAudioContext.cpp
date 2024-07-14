#include "BaseAudioContext.hpp"
#include "AudioDestinationNode.hpp"
#include "AudioEngine.hpp"
#include "timer.hpp"

namespace js_audio {

BaseAudioContext::BaseAudioContext()
    : audio_destination_node_ptr_(std::make_shared<AudioDestinationNode>()), construct_microsecond_time_(
          GetCurrentSystemTime<double>(TimeUnit::MICORSECOND)) {
  if (auto audio_engine_ptr = AudioEngine::Get().lock()) {
    sample_rate_ =
        audio_engine_ptr->audio_player_config().sample_rate_milli_hz / 1e3;
  }
}

std::shared_ptr<AudioDestinationNode>
BaseAudioContext::audio_destination_node_ptr() {
  return audio_destination_node_ptr_;
}

float BaseAudioContext::sample_rate() const { return sample_rate_; }

double BaseAudioContext::GetCurrentTime() {
  double micro_diff = GetCurrentSystemTime<double>(TimeUnit::MICORSECOND) -
                      construct_microsecond_time_;
  return ConvertFromLeftTimeUnitToRight<TimeUnit::MICORSECOND,
                                        TimeUnit::SECOND>(micro_diff);
}

} // namespace js_audio
