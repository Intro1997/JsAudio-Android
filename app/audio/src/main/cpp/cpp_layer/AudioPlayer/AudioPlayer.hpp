#pragma once
#include "BaseAudioContext.hpp"
#include <SLES/OpenSLES.h>
#include <mutex>
#include <tuple>
#include <vector>

namespace js_audio {
struct AudioPlayerConfig {
  // source data buffer queue locator params
  SLuint32 num_buffers;
  // source data pcm format params
  SLuint32 source_format_type;
  SLuint32 num_channels;
  SLuint32 channel_mask;
  SLuint32 sample_rate_milli_hz;
  SLuint32 bits_per_sample;
  SLuint32 container_size;
  SLuint32 endianness;
  // sink data output mix locator params
  SLuint32 sink_locator_type;
  uint32_t samples_per_buffer;
};

class AudioPlayer {
public:
  AudioPlayer(const AudioPlayerConfig &audio_player_config); // TODO: remove it

  virtual void Start();

  virtual void Pause();

  virtual void Resume();

  virtual void Stop();

  virtual void Destroy();

  bool Valid() const;

  bool
  AddBaseAudioContext(std::weak_ptr<BaseAudioContext> base_audio_context_ptr);

protected:
  bool is_valid_;
  AudioPlayerConfig audio_player_config_;
  std::vector<std::weak_ptr<BaseAudioContext>> base_audio_context_ptr_vec_;
  std::mutex base_audio_context_vec_lock_;

private:
  AudioPlayerConfig
  ReplaceInvalidConfig(const AudioPlayerConfig &audio_player_config);

public:
  /**
   * SLEngineItf sl_engine_itf
   * SLObjectItf *mix_object_ptr
   */
  using DataLocatorOutputMixParamsType = std::tuple<SLEngineItf, SLObjectItf *>;
  /**
   * SLuint32 num_buffers
   */
  using DataLocatorBufferQueueParamsType = std::tuple<SLuint32>;

  /**
   * SLuint32 num_channels;
   * SLuint32 samples_per_sec;
   * SLuint32 bits_per_sample;
   * SLuint32 container_size;
   * SLuint32 channel_mask;
   * SLuint32 endianness;
   */
  using DataFormatPcmType =
      std::tuple<SLuint32, SLuint32, SLuint32, SLuint32, SLuint32, SLuint32>;

  /**
   * SLchar *mime_type;
   * SLuint32 container_type;
   */
  using DataFormatMimeType = std::tuple<SLchar *, SLuint32>;

  static const uint32_t kMaxSampleRate;
  static const uint32_t kMinSampleRate;
  static const AudioPlayerConfig kAudioPlayerConfig;
};
} // namespace js_audio
