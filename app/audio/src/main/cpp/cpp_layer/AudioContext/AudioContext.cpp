#include "AudioContext.hpp"
#include "AudioEngine.hpp"
#include <tuple>

using AudioEngine = js_audio::AudioEngine;
using AudioPlayerConfig = js_audio::AudioPlayerConfig;

std::tuple<uint32_t, const float> GetBaseAudioContextParams() {
  if (auto audio_engine_ptr = AudioEngine::Get().lock()) {
    AudioPlayerConfig player_config = audio_engine_ptr->audio_player_config();
    return {player_config.num_channels,
            player_config.sample_rate_milli_hz / 1e3};
  }
  return {0, 0};
}

namespace js_audio {

AudioContext::AudioContext() : BaseAudioContext(GetBaseAudioContextParams()) {}

} // namespace js_audio
