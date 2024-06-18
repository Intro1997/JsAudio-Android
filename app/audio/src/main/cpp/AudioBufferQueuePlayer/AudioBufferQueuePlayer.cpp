#include "AudioBufferQueuePlayer.hpp"
#include "SLES/OpenSLES_Android.h"
#include "checker.hpp"
#include "logger.hpp"
#include <cmath>
#include <tuple>
#include <type_traits>

static void *CreateSlPCMDataFormat(size_t params_cnt, ...);

static void *CreateBufferQueueSlDataLocator(size_t params_cnt, ...);

static void *CreateOutputMixSlDataLocator(size_t params_cnt, ...);

#define CHECK_TYPE(left, right) (typeid(left).name() == typeid(right).name())

template <typename... Args>
static std::tuple<std::decay_t<Args>...>
DecayTuple(const std::tuple<Args...> &) {
  return std::make_tuple(std::decay_t<Args>{}...);
}

static void AudioPlayerBufferCallback(SLBufferQueueItf sl_buffer_queue_itf,
                                      void *context);

#define PCM_SAMPLE_LENGTH 9
#define PCM_SAMPLE_CHANNEL 2
#define AUDIO_DATA_STORAGE_SIZE (44100 * PCM_SAMPLE_CHANNEL * PCM_SAMPLE_LENGTH)
#define AUDIO_DATA_BUFFER_SIZE (44100)
#define DOUBLE_M_PI (M_PI * 2)
#define DO_TONE_FREQ (261.6)
#define REI_TONE_FREQ (293.6)
#define MI_TONE_FREQ (329.6)
#define FA_TONE_FREQ (349.2)
#define SOL_TONE_FREQ (392)
#define DETUNE (0)

static SLint16 *InitPcmData() {
  auto pcm_data = new SLint16[AUDIO_DATA_STORAGE_SIZE]();

  const double sine_list[] = {
      DOUBLE_M_PI * (DO_TONE_FREQ * std::pow(2.0, DETUNE / 1200.0)) / 44100.0,
      DOUBLE_M_PI * (REI_TONE_FREQ * std::pow(2.0, DETUNE / 1200.0)) / 44100.0,
      DOUBLE_M_PI * (MI_TONE_FREQ * std::pow(2.0, DETUNE / 1200.0)) / 44100.0,
      DOUBLE_M_PI * (FA_TONE_FREQ * std::pow(2.0, DETUNE / 1200.0)) / 44100.0,
      DOUBLE_M_PI * (SOL_TONE_FREQ * std::pow(2.0, DETUNE / 1200.0)) / 44100.0,
  };

  uint32_t start = 0;
  uint32_t end = 44100;
  for (uint32_t j = 0; j < PCM_SAMPLE_LENGTH; j++) {
    uint32_t idx = j > 4 ? (8 - j) : j;
    for (uint32_t i = start; i < end; i++) {
      double ret = sin(sine_list[idx] * (double)i);
      pcm_data[i * 2] = pcm_data[i * 2 + 1] = ret * INT16_MAX;
    }
    start = end;
    end += 44100;
  }
  return pcm_data;
}

namespace js_audio {

AudioBufferQueuePlayer::AudioBufferQueuePlayer(
    /* source data buffer queue locator params */ SLuint32 num_buffers,
    /* source data pcm format params */ SLuint32 source_format_type,
    SLuint32 num_channels, SLuint32 samples_per_sec, SLuint32 bits_per_sample,
    SLuint32 container_size, SLuint32 channel_mask, SLuint32 endianness,
    /* sink data output mix locator params */ SLuint32 sink_locator_type,
    std::shared_ptr<AudioEngine> audio_engine)
    : AudioPlayer(), data_source_locator_(nullptr),
      data_source_format_(nullptr),
      data_source_format_type_(source_format_type), data_sink_locator_(nullptr),
      data_sink_locator_type_(sink_locator_type), audio_data_buffer_(nullptr),
      audio_data_buffer_size_(0), current_play_size_(0) {

  SLEngineItf sl_engine_itf = audio_engine->sl_engine_interface_;
  if (!sl_engine_itf) {
    LOGE("Invalid engine interface!\n");
    return;
  }

  do {
    data_source_locator_ =
        CreateDataLocator(data_source_locator_type_, num_buffers);
    if (data_source_locator_ == nullptr) {
      LOGE("Create data source locator failed\n");
      break;
    }
    data_source_format_ = CreateDataFormat(
        source_format_type, num_channels, samples_per_sec, bits_per_sample,
        container_size, channel_mask, endianness);
    if (data_source_format_ == nullptr) {
      LOGE("Create data source format failed\n");
      break;
    }
    data_sink_locator_ = CreateDataLocator(sink_locator_type, sl_engine_itf,
                                           &sl_outputmix_object_);
    if (data_sink_locator_ == nullptr) {
      LOGE("Create data sink locator failed\n");
      break;
    }
  } while (false);

  if (!data_source_locator_ || !data_source_format_ || !data_sink_locator_) {
    AudioBufferQueuePlayer::Destroy();
    return;
  }

  SLuint32 num_interfaces = 2;
  SLInterfaceID interface_array[] = {SL_IID_PLAY, SL_IID_BUFFERQUEUE};
  SLboolean interface_required[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
  sl_data_source_ = {data_source_locator_, data_source_format_};
  sl_data_sink_ = {data_sink_locator_, nullptr};

  SLresult result =
      (*sl_engine_itf)
          ->CreateAudioPlayer(sl_engine_itf, &sl_player_object_,
                              &sl_data_source_, &sl_data_sink_, num_interfaces,
                              interface_array, interface_required);
  if (result != SL_RESULT_SUCCESS) {
    LOGE("Create audio player failed! Error code: 0x%x\n", result);
    AudioBufferQueuePlayer::Destroy();
    return;
  }

  result = (*sl_player_object_)->Realize(sl_player_object_, SL_BOOLEAN_FALSE);

  if (result != SL_RESULT_SUCCESS) {
    LOGE("Realize audio player failed! Error code: 0x%x\n", result);
    AudioBufferQueuePlayer::Destroy();
    return;
  }

  result = (*sl_player_object_)
               ->GetInterface(sl_player_object_, SL_IID_BUFFERQUEUE,
                              &sl_player_source_buffer_itf_);

  if (result != SL_RESULT_SUCCESS) {
    LOGE("Get audio player interface failed! Error code: 0x%x\n", result);
    AudioBufferQueuePlayer::Destroy();
    return;
  }

  result = (*sl_player_source_buffer_itf_)
               ->RegisterCallback(sl_player_source_buffer_itf_,
                                  AudioPlayerBufferCallback, this);

  if (result != SL_RESULT_SUCCESS) {
    LOGE("Get audio player source buffer interface failed! Error code: 0x%x\n",
         result);
    AudioBufferQueuePlayer::Destroy();
    return;
  }

  result = (*sl_player_object_)
               ->GetInterface(sl_player_object_, SL_IID_PLAY, &sl_player_itf_);

  if (result != SL_RESULT_SUCCESS) {
    LOGE("Get audio player interface failed! Error code: 0x%x\n", result);
    AudioBufferQueuePlayer::Destroy();
    return;
  }

  audio_data_buffer_ = InitPcmData();
  audio_data_buffer_size_ = AUDIO_DATA_STORAGE_SIZE;

  is_valid_ = true;
}

bool AudioBufferQueuePlayer::UpdateAudioData(SLint16 *audio_data_pointer,
                                             size_t data_size) {
  if (audio_data_pointer == nullptr) {
    return false;
  }
  audio_data_buffer_ = new SLint16[data_size]();
  std::memcpy(audio_data_buffer_, audio_data_pointer,
              data_size * sizeof(SLint16));
  audio_data_buffer_size_ = data_size;
  return true;
}

bool AudioBufferQueuePlayer::GetAudioDataCopied(void *receive_ptr,
                                                size_t start_bytes,
                                                size_t copy_length) {
  if (receive_ptr == nullptr) {
    LOGE("Receive pointer is invalid!\n");
    return false;
  }

  size_t audio_data_buffer_bytes = audio_data_buffer_size_ * sizeof(SLint16);
  if (start_bytes >= audio_data_buffer_bytes ||
      copy_length >= audio_data_buffer_bytes - start_bytes) {
    LOGE("Offset is valid, cannot copy %zu bytes data start with %zu from %zu "
         "origin data bytes.\n",
         copy_length, start_bytes, audio_data_buffer_bytes);
    return false;
  }

  char *char_buffer_ptr = (char *)audio_data_buffer_;
  std::memcpy(receive_ptr, char_buffer_ptr + start_bytes, copy_length);

  return true;
}

/**
 * return data length in bytes
 */
size_t AudioBufferQueuePlayer::GetAudioDataLength(size_t &each_element_bytes) {}

void AudioBufferQueuePlayer::Start() {
  AudioPlayer::Start();
  if (!audio_data_buffer_ || audio_data_buffer_size_ == 0) {
    LOGE("Start audio player failed! Cannot play empty audio data\n");
    return;
  }
  SLresult result;

  result =
      (*sl_player_itf_)->SetPlayState(sl_player_itf_, SL_PLAYSTATE_PLAYING);

  if (result != SL_RESULT_SUCCESS) {
    LOGE("Start audio player failed! Switch player state to playing failed, "
         "Error code: 0x%x\n",
         result);
    return;
  }

  result = (*sl_player_source_buffer_itf_)
               ->Enqueue(sl_player_source_buffer_itf_, audio_data_buffer_,
                         audio_data_buffer_size_ * sizeof(SLint16));

  if (result != SL_RESULT_SUCCESS) {
    LOGE("Start audio player failed! Enqueue audio data to player failed! "
         "Error code: 0x%x\n",
         result);
    return;
  }
}

void AudioBufferQueuePlayer::Destroy() {
  AudioPlayer::Destroy();
  ReleaseDataSink();
  ReleaseDataSource();
}

template <typename... Args>
void *AudioBufferQueuePlayer::CreateDataLocator(SLuint32 data_locator_type,
                                                Args... args) {
  std::tuple params = std::make_tuple(args...);
  size_t params_cnt = std::tuple_size<decltype(params)>::value;
  switch (data_locator_type) {
  case SL_DATALOCATOR_BUFFERQUEUE: {
    if (!CHECK_TYPE(decltype(DecayTuple(params)),
                    js_audio::AudioPlayer::DataLocatorBufferQueueParamsType)) {
      LOGE("Create SL_DATALOCATOR_BUFFERQUEUE failed! Need parameter: "
           "{SLuint32}\n");
      return nullptr;
    }
    return CreateBufferQueueSlDataLocator(params_cnt, args...);
  }
  case SL_DATALOCATOR_OUTPUTMIX: {
    if (!CHECK_TYPE(decltype(DecayTuple(params)),
                    js_audio::AudioPlayer::DataLocatorOutputMixParamsType)) {
      LOGE("Create SL_DATALOCATOR_OUTPUTMIX failed! Valid paramter is "
           "<SLEngineItf, SLObjectItf *, SLuint32, SLInterfaceID *, SLboolean "
           "*>\n");
      return nullptr;
    }
    return CreateOutputMixSlDataLocator(params_cnt, args...);
  }
  default: {
    LOGE("Error: Unknwon data locator type!\n");
    return nullptr;
  }
  }
}

template <typename... Args>
void *AudioBufferQueuePlayer::CreateDataFormat(SLuint32 format_type,
                                               Args... args) {

  std::tuple params = std::make_tuple(args...);
  switch (format_type) {
  case SL_DATAFORMAT_PCM: {
    if (!CHECK_TYPE(decltype(params),
                    js_audio::AudioPlayer::DataFormatPcmType)) {
      LOGE("Create SL_DATAFORMAT_PCM failed! Valid parameter is <SLuint32, "
           "SLuint32, SLuint32, SLuint32, SLuint32, SLuint32>\n");
      return nullptr;
    }
    return CreateSlPCMDataFormat(std::tuple_size<decltype(params)>::value,
                                 args...);
  }
  default: {
    LOGE("Unknown data format type!\n");
    return nullptr;
  }
  }
}

void AudioBufferQueuePlayer::ReleaseDataSource() {
  if (data_source_locator_) {
    switch (data_source_locator_type_) {
    case SL_DATALOCATOR_BUFFERQUEUE: {
      SLDataLocator_BufferQueue *handle =
          (SLDataLocator_BufferQueue *)data_source_locator_;
      delete handle;
      data_source_locator_ = nullptr;
      break;
    }
    default: {
      LOGE("ERROR!!! Cannot find correct way to release DATA SOURCE LOCATOR "
           "pointer!\n");
      break;
    }
    }
  }
  if (data_source_format_) {
    switch (data_source_format_type_) {
    case SL_DATAFORMAT_PCM: {
      SLDataFormat_PCM *handle = (SLDataFormat_PCM *)data_source_format_;
      delete handle;
      data_source_format_ = nullptr;
      break;
    }
    default: {
      LOGE("ERROR!!! Cannot find correct way to release DATA SOURCE FORMAT "
           "pointer!\n");
      break;
    }
    }
  }
}

void AudioBufferQueuePlayer::ReleaseDataSink() {
  if (data_sink_locator_) {
    switch (data_sink_locator_type_) {
    case SL_DATALOCATOR_OUTPUTMIX: {
      SLDataLocator_OutputMix *handle =
          (SLDataLocator_OutputMix *)data_sink_locator_;
      delete handle;
      data_sink_locator_ = nullptr;
      break;
    }
    default: {
      LOGE("ERROR!!! Cannot find correct way to release DATA SINK LOCATOR "
           "pointer!\n");
      break;
    }
    }
  }
}

} // namespace js_audio

static void *CreateSlPCMDataFormat(size_t params_cnt, ...) {
  SLDataFormat_PCM *pcm_format = new SLDataFormat_PCM();

  va_list valist;
  va_start(valist, params_cnt);
  pcm_format->formatType = SL_DATAFORMAT_PCM;
  pcm_format->numChannels = va_arg(valist, SLuint32);
  pcm_format->samplesPerSec = va_arg(valist, SLuint32);
  pcm_format->bitsPerSample = va_arg(valist, SLuint32);
  pcm_format->containerSize = va_arg(valist, SLuint32);
  pcm_format->channelMask = va_arg(valist, SLuint32);
  pcm_format->endianness = va_arg(valist, SLuint32);
  va_end(valist);

  return pcm_format;
}

static void *CreateBufferQueueSlDataLocator(size_t params_cnt, ...) {
  va_list valist;
  va_start(valist, params_cnt);
  SLDataLocator_BufferQueue *buffer_queue = new SLDataLocator_BufferQueue();
  buffer_queue->locatorType = SL_DATALOCATOR_BUFFERQUEUE;
  buffer_queue->numBuffers = va_arg(valist, SLuint32);
  va_end(valist);
  return buffer_queue;
}

static void *CreateOutputMixSlDataLocator(size_t params_cnt, ...) {
  const SLuint32 PARAMETER_ERROR = -1;
  SLresult result = PARAMETER_ERROR;

  do {
    va_list valist;
    va_start(valist, params_cnt);
    SLEngineItf sl_engine_itf = va_arg(valist, SLEngineItf);
    SLObjectItf *mix_object_ptr = va_arg(valist, SLObjectItf *);
    va_end(valist);

    SLuint32 num_interfaces = 1;
    SLInterfaceID interface_array_ptr[] = {SL_IID_OUTPUTMIX};
    SLboolean interface_required_ptr[] = {SL_BOOLEAN_TRUE};
    result =
        (*sl_engine_itf)
            ->CreateOutputMix(sl_engine_itf, mix_object_ptr, num_interfaces,
                              interface_array_ptr, interface_required_ptr);
    if (result != SL_RESULT_SUCCESS) {
      break;
    }

    SLObjectItf mix_object = (*mix_object_ptr);
    result = (*mix_object)->Realize(mix_object, SL_BOOLEAN_FALSE);

    if (result != SL_RESULT_SUCCESS) {
      break;
    }

    SLDataLocator_OutputMix *output_mix_locator = new SLDataLocator_OutputMix();
    output_mix_locator->locatorType = SL_DATALOCATOR_OUTPUTMIX;
    output_mix_locator->outputMix = mix_object;
    return output_mix_locator;
  } while (false);

  if (result == PARAMETER_ERROR) {
    LOGE("Create SL_DATALOCATOR_OUTPUTMIX failed! Valid paramter is "
         "<SLEngineItf, SLObjectItf *, SLuint32, SLInterfaceID *, SLboolean "
         "*>\n");
  } else {
    LOGE("Create SL_DATALOCATOR_OUTPUTMIX failed! Error code: 0x%x\n", result);
  }
  return nullptr;
}

static void AudioPlayerBufferCallback(SLBufferQueueItf sl_buffer_queue_itf,
                                      void *context) {
  auto audio_player_ptr =
      static_cast<js_audio::AudioBufferQueuePlayer *>(context);
  LOGE("Get in audio player buffer callback\n");
  // size_t player
  // (*sl_buffer_queue_itf)->Enqueue(sl_buffer_queue_itf, )
}