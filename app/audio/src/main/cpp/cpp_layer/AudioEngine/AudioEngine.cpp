#include "AudioEngine.hpp"
#include "AudioBufferQueuePlayer.hpp"
#include "checker.hpp"
#include "logger.hpp"
#include <SLES/OpenSLES_Android.h>
#include <cmath>

#define AUDIO_DATA_STORAGE_SIZE (44100 * 2 * 9)
#define AUDIO_DATA_BUFFER_SIZE (44100)
#define DOUBLE_M_PI (M_PI * 2)
#define DO_TONE_FREQ (261.6)
#define REI_TONE_FREQ (293.6)
#define MI_TONE_FREQ (329.6)
#define FA_TONE_FREQ (349.2)
#define SOL_TONE_FREQ (392)
#define DETUNE (0)

namespace js_audio {
struct AudioEngineConstructHelper : public AudioEngine {};

SLint16 *pcm_data = new SLint16[AUDIO_DATA_STORAGE_SIZE]();
uint32_t pcm_data_offset = 0;

void InitPcmData() {

  const double sine_list[] = {
      DOUBLE_M_PI * (DO_TONE_FREQ * std::pow(2.0, DETUNE / 1200.0)) / 44100.0,
      DOUBLE_M_PI * (REI_TONE_FREQ * std::pow(2.0, DETUNE / 1200.0)) / 44100.0,
      DOUBLE_M_PI * (MI_TONE_FREQ * std::pow(2.0, DETUNE / 1200.0)) / 44100.0,
      DOUBLE_M_PI * (FA_TONE_FREQ * std::pow(2.0, DETUNE / 1200.0)) / 44100.0,
      DOUBLE_M_PI * (SOL_TONE_FREQ * std::pow(2.0, DETUNE / 1200.0)) / 44100.0,
  };

  uint32_t start = 0;
  uint32_t end = 44100;
  for (uint32_t j = 0; j < 9; j++) {
    uint32_t idx = j > 4 ? (8 - j) : j;
    for (uint32_t i = start; i < end; i++) {
      double ret = sin(sine_list[idx] * (double)i);
      pcm_data[i * 2] = pcm_data[i * 2 + 1] = ret * INT16_MAX;
    }
    start = end;
    end += 44100;
  }
}

void DeletePcmData() { delete[] pcm_data; }

void AudioPlayerCallback(SLBufferQueueItf sl_buffer_queue_itf, void *pContext) {
  LOGD("I AM HUNGRAY!!!!!!!!\n");
  if (pcm_data_offset + 2 * AUDIO_DATA_BUFFER_SIZE > AUDIO_DATA_STORAGE_SIZE) {
    LOGD("Finish load all data in storage!\n");
    pcm_data_offset = 0;
  }
  (*sl_buffer_queue_itf)
      ->Enqueue(sl_buffer_queue_itf, pcm_data + pcm_data_offset,
                2 * AUDIO_DATA_BUFFER_SIZE * sizeof(SLint16));
  pcm_data_offset += 2 * AUDIO_DATA_BUFFER_SIZE;
}

std::shared_ptr<AudioEngine> AudioEngine::audio_engine_;

std::weak_ptr<AudioEngine> AudioEngine::Get() {
  if (audio_engine_) {
    return audio_engine_;
  }

  audio_engine_ = std::make_shared<AudioEngineConstructHelper>();
  if (!audio_engine_->Init()) {
    LOGE("Init Audio Engine failed!\n");
    audio_engine_.reset();
  }

  return audio_engine_;
}

void AudioEngine::Start() {
  LOGD("Native AudioEngine::Start()\n");
  if (audio_player_) {
    audio_player_->Start();
  }
}

void AudioEngine::Pause() {
  LOGD("Native AudioEngine::Pause()\n");
  if (audio_player_) {
    audio_player_->Pause();
  }
}

void AudioEngine::Resume() {
  LOGD("Native AudioEngine::Resume()\n");
  if (audio_player_) {
    audio_player_->Resume();
  }
}

void AudioEngine::Stop() {
  LOGD("Native AudioEngine::Stop()\n");
  // audio player stop means reset play process
  // when start playing, player will play from beginning
  if (audio_player_) {
    audio_player_->Pause();
  }
}

void AudioEngine::Destroy() {
  LOGD("Native AudioEngine::Destroy()\n");
  if (audio_player_) {
    audio_player_->Destroy();
  }
}

std::weak_ptr<AudioPlayer> AudioEngine::CreateAudioBufferQueuePlayer(
    /* source data bufferqueue locator params */ SLuint32 num_buffers,
    /* source data pcm format params */ SLuint32 source_format_type,
    SLuint32 num_channels, SLuint32 samples_per_sec, SLuint32 bits_per_sample,
    SLuint32 container_size, SLuint32 channel_mask, SLuint32 endianness,
    /* sink data output mix locator params */ SLuint32 sink_locator_type) {
  audio_player_ = std::make_shared<AudioBufferQueuePlayer>(
      num_buffers, source_format_type, num_channels, samples_per_sec,
      bits_per_sample, container_size, channel_mask, endianness,
      sink_locator_type, audio_engine_);

  return audio_player_;
}

std::weak_ptr<AudioPlayer> AudioEngine::GetAudioPlayer() const {
  return audio_player_;
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
