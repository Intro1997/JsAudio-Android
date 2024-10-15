#include "AudioContext.hpp"
#include "AudioEngine.hpp"
#include "timer.hpp"
#include <tuple>

using AudioEngine = js_audio::AudioEngine;
using AudioPlayerConfig = js_audio::AudioPlayerConfig;
using ContextType = js_audio::BaseAudioContext::ContextType;

std::tuple<ContextType, uint32_t, float> GetBaseAudioContextParams() {
  if (auto audio_engine_ptr = AudioEngine::Get().lock()) {
    AudioPlayerConfig player_config = audio_engine_ptr->audio_player_config();
    return {ContextType::kOnline, player_config.num_channels,
            player_config.sample_rate_milli_hz / 1e3};
  }
  return {ContextType::kOnline, 0, 0};
}

namespace js_audio {

AudioContext::AudioContext()
    : BaseAudioContext(GetBaseAudioContextParams()),
      construct_microsecond_time_(
          GetCurrentSystemTime<double>(TimeUnit::kMicorsecond)) {}

double AudioContext::GetCurrentTime() {
  double micro_diff = GetCurrentSystemTime<double>(TimeUnit::kMicorsecond) -
                      construct_microsecond_time_;
  return ConvertFromLeftTimeUnitToRight<TimeUnit::kMicorsecond,
                                        TimeUnit::kSecond>(micro_diff);
}

} // namespace js_audio
