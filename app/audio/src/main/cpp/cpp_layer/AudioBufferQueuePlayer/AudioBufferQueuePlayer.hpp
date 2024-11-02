#pragma once
#include "AudioContext.hpp"
#include "AudioEngine.hpp"
#include "AudioPlayer.hpp"
namespace js_audio {
class AudioBufferQueuePlayer : public AudioPlayer {
public:
  AudioBufferQueuePlayer(const AudioPlayerConfig &audio_player_config,
                         std::shared_ptr<AudioEngine> audio_engine_ref);

  void Start() override;

  void Pause() override;

  void Resume() override;

  void Stop() override;

  void Destroy() override;

  /**
   * return: return byte size of a audio sample
   */
  size_t GetEachSampleByteSize();

private:
  /**
   * data_locator_type: use SLES locator type, like SL_DATALOCATOR_BUFFERQUEUE
   * args: please check AudioPlayer::XxxParametertype, like OutputMixParamsType,
   * BufferQueueParamsType etc.
   */
  template <typename... Args>
  void *CreateDataLocator(SLuint32 data_locator_type, Args... args);

  /**
   * data_locator_type: use SLES locator type, like SL_DATAFORMAT_PCM
   * args: please check AudioPlayer::XxxParametertype, like OutputMixParamsType,
   * BufferQueueParamsType etc.
   */
  template <typename... Args>
  void *CreateDataFormat(SLuint32 format_type, Args... args);

  void ReleaseSlAudioPlayer();

  void ReleaseDataSource();

  void ReleaseDataSink();

  static void AudioPlayerBufferCallback(SLBufferQueueItf sl_buffer_queue_itf,
                                        void *context);

  void ProduceSamples(size_t sample_size, std::vector<SLint16> &output);

  std::vector<SLint16> &GetCurrentBuffer();

private:
  void *data_source_locator_;
  void *data_source_format_;
  const SLuint32 data_source_locator_type_ = SL_DATALOCATOR_BUFFERQUEUE;
  SLuint32 data_source_format_type_;
  SLDataSource sl_data_source_;

  void *data_sink_locator_;
  SLObjectItf sl_outputmix_object_; // be used for data sink
  SLuint32 data_sink_locator_type_;
  SLDataSink sl_data_sink_;

  SLObjectItf sl_player_object_;
  SLBufferQueueItf sl_player_source_buffer_itf_;
  SLPlayItf sl_player_itf_;

  // TODO: make data type configeable
  std::vector<std::vector<SLint16>> sample_buffers_;
  size_t current_buffer_id_;
};

} // namespace js_audio