#pragma once
#include <SLES/OpenSLES.h>
#include <tuple>

namespace js_audio {
class AudioPlayer {
public:
  AudioPlayer(); // TODO: remove it

  virtual void Start();

  virtual void Pause();

  virtual void Resume();

  virtual void Stop();

  virtual void Destroy();

  bool Valid() const;

protected:
  bool is_valid_;

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
};
} // namespace js_audio
