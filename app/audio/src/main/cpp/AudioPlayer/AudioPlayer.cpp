#include "AudioPlayer.hpp"
#include "logger.hpp"
#include <variant>

namespace js_audio {
AudioPlayer::AudioPlayer() : is_valid_(false) {}

void AudioPlayer::Start() {}

void AudioPlayer::Destroy() { is_valid_ = false; }

bool AudioPlayer::Valid() const { return is_valid_; }

} // namespace js_audio