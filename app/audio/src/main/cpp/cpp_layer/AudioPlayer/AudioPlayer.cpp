#include "AudioPlayer.hpp"
#include "logger.hpp"
#include <variant>

namespace js_audio {
inline static SLuint32 MakeNumBuffersValid(const SLuint32 &maybe_valid);
inline static SLuint32 MakeSourceFormatTypeValid(const SLuint32 &maybe_valid);
inline static SLuint32 MakeNumChannelsValid(const SLuint32 &maybe_valid);
inline static SLuint32 MakeChannelMaskValid(const SLuint32 &maybe_valid);
inline static SLuint32 MakeSampleRateMilliHzValid(const SLuint32 &maybe_valid);
inline static SLuint32 MakeBitsPerSampleValid(const SLuint32 &maybe_valid);
inline static SLuint32 MakeContainerSizeValid(const SLuint32 &maybe_valid);
inline static SLuint32 MakeEndiannessValid(const SLuint32 &maybe_valid);
inline static SLuint32 MakeSinkLocatorTypeValid(const SLuint32 &maybe_valid);
inline static uint32_t MakeSamplesPerBufferValid(const uint32_t &maybe_valid);

const uint32_t AudioPlayer::kMaxSampleRate = 100;
const uint32_t AudioPlayer::kMinSampleRate = 8192;

// clang-format off
const AudioPlayerConfig AudioPlayer::kAudioPlayerConfig = {
    /* num_buffers */           4,
    /* source_format_type */    SL_DATAFORMAT_PCM,
    /* num_channels */          2,
    /* channel_mask */          SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
    /* sample_rate_milli_hz */  SL_SAMPLINGRATE_44_1,
    /* bits_per_sample */       SL_PCMSAMPLEFORMAT_FIXED_16,
    /* container_size */        SL_PCMSAMPLEFORMAT_FIXED_16,
    /* endianness */            SL_BYTEORDER_LITTLEENDIAN,
    /* sink_locator_type */     SL_DATALOCATOR_OUTPUTMIX,
    /* samples_per_buffer */    4096
};
// clang-format on

AudioPlayer::AudioPlayer(const AudioPlayerConfig &audio_player_config)
    : is_valid_(false),
      audio_player_config_(ReplaceInvalidConfig(audio_player_config)) {}

void AudioPlayer::Start() {}

void AudioPlayer::Pause() {}

void AudioPlayer::Resume() {}

void AudioPlayer::Stop() {}

void AudioPlayer::Destroy() { is_valid_ = false; }

bool AudioPlayer::Valid() const { return is_valid_; }

bool AudioPlayer::AddBaseAudioContext(
    std::weak_ptr<BaseAudioContext> base_audio_context) {
  if (base_audio_context.lock()) {
    base_audio_context_vec_.push_back(base_audio_context);
    return true;
  }
  return false;
}

AudioPlayerConfig AudioPlayer::ReplaceInvalidConfig(
    const AudioPlayerConfig &audio_player_config) {
  return {
      MakeNumBuffersValid(audio_player_config.num_buffers),
      MakeSourceFormatTypeValid(audio_player_config.source_format_type),
      MakeNumChannelsValid(audio_player_config.num_channels),
      MakeChannelMaskValid(audio_player_config.channel_mask),
      MakeSampleRateMilliHzValid(audio_player_config.sample_rate_milli_hz),
      MakeBitsPerSampleValid(audio_player_config.bits_per_sample),
      MakeContainerSizeValid(audio_player_config.container_size),
      MakeEndiannessValid(audio_player_config.endianness),
      MakeSinkLocatorTypeValid(audio_player_config.sink_locator_type),
      MakeSamplesPerBufferValid(audio_player_config.samples_per_buffer),
  };
}

inline SLuint32 MakeNumBuffersValid(const SLuint32 &maybe_valid) {
  return maybe_valid > 0 ? maybe_valid
                         : AudioPlayer::kAudioPlayerConfig.num_buffers;
}

inline SLuint32 MakeSourceFormatTypeValid(const SLuint32 &maybe_valid) {
  // must be SL_DATAFORMAT_PCM
  return AudioPlayer::kAudioPlayerConfig.source_format_type;
}

inline SLuint32 MakeNumChannelsValid(const SLuint32 &maybe_valid) {
  // must be 2
  return AudioPlayer::kAudioPlayerConfig.num_channels;
}

inline SLuint32 MakeChannelMaskValid(const SLuint32 &maybe_valid) {
  // cannot detect if valid
  return maybe_valid;
}

inline SLuint32 MakeSampleRateMilliHzValid(const SLuint32 &maybe_valid) {
  // clang-format off
  if (
        maybe_valid == SL_SAMPLINGRATE_8 ||
        maybe_valid == SL_SAMPLINGRATE_11_025 ||
        maybe_valid == SL_SAMPLINGRATE_12 ||
        maybe_valid == SL_SAMPLINGRATE_16 ||
        maybe_valid == SL_SAMPLINGRATE_22_05 ||
        maybe_valid == SL_SAMPLINGRATE_24 ||
        maybe_valid == SL_SAMPLINGRATE_32 ||
        maybe_valid == SL_SAMPLINGRATE_44_1 ||
        maybe_valid == SL_SAMPLINGRATE_48 ||
        maybe_valid == SL_SAMPLINGRATE_64 ||
        maybe_valid == SL_SAMPLINGRATE_88_2 ||
        maybe_valid == SL_SAMPLINGRATE_96 ||
        maybe_valid == SL_SAMPLINGRATE_192
  ) {
    return maybe_valid;
  }
  // clang-format on
  return SL_SAMPLINGRATE_44_1;
}

inline SLuint32 MakeBitsPerSampleValid(const SLuint32 &maybe_valid) {
  // must be SL_PCMSAMPLEFORMAT_FIXED_16
  return SL_PCMSAMPLEFORMAT_FIXED_16;
}

inline SLuint32 MakeContainerSizeValid(const SLuint32 &maybe_valid) {
  // must be SL_PCMSAMPLEFORMAT_FIXED_16
  return SL_PCMSAMPLEFORMAT_FIXED_16;
}

inline SLuint32 MakeEndiannessValid(const SLuint32 &maybe_valid) {
  if (maybe_valid == SL_BYTEORDER_BIGENDIAN ||
      maybe_valid == SL_BYTEORDER_LITTLEENDIAN) {
    return maybe_valid;
  }
  // TODO: maybe issues here, get actual byteorder
  // in os
  return SL_BYTEORDER_LITTLEENDIAN;
}

inline SLuint32 MakeSinkLocatorTypeValid(const SLuint32 &maybe_valid) {
  // must be SL_DATALOCATOR_OUTPUTMIX
  return SL_DATALOCATOR_OUTPUTMIX;
}

inline uint32_t MakeSamplesPerBufferValid(const uint32_t &maybe_valid) {
  return std::clamp(maybe_valid, AudioPlayer::kMinSampleRate,
                    AudioPlayer::kMaxSampleRate);
}

} // namespace js_audio