#include "AudioEngine.hpp"
#include "AudioBufferQueuePlayer.hpp"
#include "checker.hpp"
#include "logger.hpp"
#include <SLES/OpenSLES_Android.h>
#include <cmath>

namespace js_audio {
// we do not want user use AudioEngine constructor, and
// make it protected, this makes AudioEngine constructor
// is invisible to std::make_shared. So we use
// AudioEngineConstructHelper to creat AudioEngine
struct AudioEngineConstructHelper : public AudioEngine {};

AudioPlayerConfig AudioEngine::audio_player_config_ =
    AudioPlayer::kAudioPlayerConfig;
std::shared_ptr<AudioEngine> AudioEngine::audio_engine_ptr_ = nullptr;

std::weak_ptr<AudioEngine> AudioEngine::Get() {
  if (audio_engine_ptr_) {
    return audio_engine_ptr_;
  }

  audio_engine_ptr_ = std::make_shared<AudioEngineConstructHelper>();

  if (!audio_engine_ptr_->Init()) {
    LOGE("Init Audio Engine failed!\n");
    audio_engine_ptr_.reset();
  }

  return audio_engine_ptr_;
}

void AudioEngine::Start() {
  LOGD("Native AudioEngine::Start()\n");
  if (audio_buffer_queue_player_) {
    audio_buffer_queue_player_->Start();
  }
}

void AudioEngine::Pause() {
  LOGD("Native AudioEngine::Pause()\n");
  if (audio_buffer_queue_player_) {
    audio_buffer_queue_player_->Pause();
  }
}

void AudioEngine::Resume() {
  LOGD("Native AudioEngine::Resume()\n");
  if (audio_buffer_queue_player_) {
    audio_buffer_queue_player_->Resume();
  }
}

void AudioEngine::Stop() {
  LOGD("Native AudioEngine::Stop()\n");
  if (audio_buffer_queue_player_) {
    audio_buffer_queue_player_->Stop();
  }
}

void AudioEngine::Destroy() {
  LOGD("Native AudioEngine::Destroy()\n");
  if (audio_buffer_queue_player_) {
    audio_buffer_queue_player_->Destroy();
  }
}

void AudioEngine::InitAudioBufferQueuePlayer() {
  audio_buffer_queue_player_ = std::make_shared<AudioBufferQueuePlayer>(
      audio_player_config_, audio_engine_ptr_);
}

std::weak_ptr<AudioPlayer>
AudioEngine::GetAudioPlayer(const AudioPlayerType &audio_player_type) {
  switch (audio_player_type) {
  case AudioPlayerType::kBufferQueuePlayer: {
    if (!audio_buffer_queue_player_) {
      InitAudioBufferQueuePlayer();
    }
    return audio_buffer_queue_player_;
  }
  }
  return {};
}

AudioPlayerConfig AudioEngine::audio_player_config() {
  return audio_player_config_;
}

void AudioEngine::set_audio_player_config(
    const AudioPlayerConfig &audio_player_config) {
  audio_player_config_ = audio_player_config;
}

bool AudioEngine::Init() {
  SLresult result = SL_RESULT_SUCCESS;
  SLEngineOption sl_engine_option[2] = {
      {SL_ENGINEOPTION_THREADSAFE, SL_BOOLEAN_TRUE},
      {SL_ENGINEOPTION_LOSSOFCONTROL, SL_BOOLEAN_FALSE},
  };
  SLInterfaceID sl_engine_interface_ids[] = {SL_IID_ENGINE};
  SLboolean sl_engine_interface_require[] = {SL_BOOLEAN_TRUE};
  result = slCreateEngine(&sl_engine_object_, 2, sl_engine_option, 1,
                          sl_engine_interface_ids, sl_engine_interface_require);

  CHECK_WITH_RET_CALL((result == SL_RESULT_SUCCESS), false,
                      LOGE("Create SLEngine failed! Error Code: 0x%x", result));

  result = (*sl_engine_object_)->Realize(sl_engine_object_, SL_BOOLEAN_TRUE);

  CHECK_WITH_RET_CALL(
      (result == SL_RESULT_SUCCESS), false,
      LOGE("Realize SLEngine failed! Error Code: 0x%x", result));

  result = (*sl_engine_object_)
               ->GetInterface(sl_engine_object_, SL_IID_ENGINE,
                              &sl_engine_interface_);

  CHECK_WITH_RET_CALL(
      (result == SL_RESULT_SUCCESS), false,
      LOGE("Get SLEngineInterface failed! Error Code: 0x%x", result));
  return true;
}

AudioEngine::~AudioEngine() {}

} // namespace js_audio
