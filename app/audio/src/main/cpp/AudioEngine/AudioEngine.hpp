#pragma once
#include "AudioContext.hpp"
#include "AudioPlayer.hpp"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <memory>
#include <vector>

namespace js_audio {

void DeletePcmData(); // TODO: just for test, remove it!

class AudioEngine {
  friend class AudioBufferQueuePlayer;

public:
  ~AudioEngine();

  static std::weak_ptr<AudioEngine> Get();

  void Start();
  void Pause();
  void Resume();
  void Stop();
  void Destroy();

  std::weak_ptr<AudioPlayer> CreateAudioBufferQueuePlayer(
      /* source data bufferqueue locator params */ SLuint32 num_buffers,
      /* source data pcm format params */ SLuint32 source_format_type,
      SLuint32 num_channels, SLuint32 samples_per_sec, SLuint32 bits_per_sample,
      SLuint32 container_size, SLuint32 channel_mask, SLuint32 endianness,
      /* sink data output mix locator params */ SLuint32 sink_locator_type);

  std::weak_ptr<AudioPlayer> GetAudioPlayer() const;

protected:
  AudioEngine() = default;

private:
  bool Init();

  static std::shared_ptr<AudioEngine> audio_engine_;
  SLObjectItf sl_engine_object_;
  SLEngineItf sl_engine_interface_;

  std::shared_ptr<AudioPlayer> audio_player_;
};
}; // namespace js_audio