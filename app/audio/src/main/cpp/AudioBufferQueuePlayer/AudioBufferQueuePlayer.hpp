#pragma once
#include "AudioEngine.hpp"
#include "AudioPlayer.hpp"
namespace js_audio {
class AudioBufferQueuePlayer : public AudioPlayer {
public:
  AudioBufferQueuePlayer(
      /* source data bufferqueue locator params */ SLuint32 num_buffers,
      /* source data pcm format params */ SLuint32 source_format_type,
      SLuint32 num_channels, SLuint32 samples_per_sec, SLuint32 bits_per_sample,
      SLuint32 container_size, SLuint32 channel_mask, SLuint32 endianness,
      /* sink data output mix locator params */
      SLuint32 sink_locator_type, std::shared_ptr<AudioEngine> audio_engine);

  /**
   * audio_data_pointer: a pointer to audio data.
   * data_size: size of element, not bytes.
   */
  bool UpdateAudioData(SLint16 *audio_data_pointer, size_t data_size);

  /**
   * receive_ptr: a pointer to receive audio data
   * start_bytes: set offset to starting copy in bytes
   * copy_length: set total copy length in bytes
   */
  bool GetAudioDataCopied(void *receive_ptr, size_t start_bytes,
                          size_t copy_length);

  void Start() override;

  void Pause() override;

  void Resume() override;

  void Stop() override;

  void Destroy() override;

  bool IsEnd();

  /**
   * return: return offset elements, not BYTES;
   */
  size_t GetCurrentOffset();

  /**
   * offset: elements offset, not BYTES;
   */
  void SetCurrentOffset(size_t offset);

  /**
   * return: return data length in element size, not BYTES
   */
  size_t GetAudioDataTotalSize();

  /**
   * return: return byte size of a audio sample
   */
  size_t GetEachSampleByteSize();

  void* GetAudioDataPointer();

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

  void ReleaseAudioData();

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

  SLint16 *audio_data_buffer_;
  size_t audio_data_buffer_size_; // element size, not bytes
  size_t current_play_offset_;    // playing progress, not in bytes
};

} // namespace js_audio