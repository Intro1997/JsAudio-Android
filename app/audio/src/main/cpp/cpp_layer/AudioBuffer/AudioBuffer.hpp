#pragma once
#include <cstdint>
#include <vector>

namespace js_audio {
class AudioBuffer {
public:
  AudioBuffer(const uint32_t &number_of_channel, const uint32_t &length,
              const float &sample_rate);
  AudioBuffer(std::vector<std::vector<float>> &&src);

  uint32_t number_of_channel() const;
  uint32_t length() const;
  float sample_rate() const;
  double duration() const;

  void CopyToChannel(const float *source_ptr, const uint32_t &source_size,
                     const uint32_t channel_number,
                     const uint32_t &src_start_idx);
  void CopyFromChannel(float *dest_ptr, const uint32_t &dest_size,
                       const uint32_t channel_number,
                       const uint32_t &src_start_idx) const;

  std::vector<float> GetChannelData(const uint32_t channel) const;

private:
  uint32_t number_of_channel_;
  uint32_t length_;
  float sample_rate_;
  double duration_;
  std::vector<std::vector<float>> audio_channel_buffers_;
};
}; // namespace js_audio