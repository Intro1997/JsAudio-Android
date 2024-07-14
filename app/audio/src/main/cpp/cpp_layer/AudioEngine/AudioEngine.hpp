#pragma once
#include "AudioContext.hpp"
#include "AudioPlayer.hpp"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <memory>
#include <vector>

namespace js_audio {

enum class AudioPlayerType { kBufferQueuePlayer = 0 };

class AudioEngine {
  friend class AudioBufferQueuePlayer;

public:
  ~AudioEngine();

  static std::weak_ptr<AudioEngine> Get();
  static void SetPlayerSampleRate(const uint32_t &sample_rate);
  static void SetPlayerFramesPerBuffer(const uint32_t &frames_per_buffer);

  void Start();
  void Pause();
  void Resume();
  void Stop();
  void Destroy();

  std::weak_ptr<AudioPlayer>
  GetAudioPlayer(const AudioPlayerType &audio_player_type);

  static AudioPlayerConfig audio_player_config();
  static void
  set_audio_player_config(const AudioPlayerConfig &audio_player_config);

protected:
  AudioEngine() = default;

private:
  bool Init();

  void InitAudioBufferQueuePlayer();

  static std::shared_ptr<AudioEngine> audio_engine_ptr_;
  static AudioPlayerConfig audio_player_config_;

  SLObjectItf sl_engine_object_;
  SLEngineItf sl_engine_interface_;

  std::shared_ptr<AudioPlayer> audio_buffer_queue_player_;
};
}; // namespace js_audio