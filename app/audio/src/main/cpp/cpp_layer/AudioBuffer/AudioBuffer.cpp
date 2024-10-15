#include "AudioBuffer.hpp"
#include "logger.hpp"
#include <cstdint>
namespace js_audio {
AudioBuffer::AudioBuffer(const uint32_t &number_of_channel,
                         const uint32_t &length, const float &sample_rate)
    : number_of_channel_(number_of_channel), length_(length),
      sample_rate_(sample_rate) {
  // TODO: maybe need do validation and throw here
  if (sample_rate_ == 0) {
    duration_ = 0;
  } else {
    duration_ = ((double)length_ / (double)sample_rate_);
  }
  audio_channel_buffers_.resize(number_of_channel_);
  for (auto &channel_buffer : audio_channel_buffers_) {
    channel_buffer.resize(length_);
  }
}

AudioBuffer::AudioBuffer(std::vector<std::vector<float>> &&src)
    : number_of_channel_(src.size()), audio_channel_buffers_(std::move(src)) {
  if (audio_channel_buffers_.size()) {
    length_ = audio_channel_buffers_[0].size();
  }
}

uint32_t AudioBuffer::number_of_channel() const { return number_of_channel_; }

uint32_t AudioBuffer::length() const { return length_; }

float AudioBuffer::sample_rate() const { return sample_rate_; }

double AudioBuffer::duration() const { return duration_; }

void AudioBuffer::CopyToChannel(const float *source_ptr,
                                const uint32_t &source_size,
                                const uint32_t channel_number,
                                const uint32_t &src_start_idx) {
  if (source_ptr == nullptr || src_start_idx > (length() - 1)) {
    // TODO: maybe throw here?
    return;
  }
  try {
    std::vector<float> &channel_buffer =
        audio_channel_buffers_.at(channel_number);

    uint32_t src_end_idx =
        std::min(src_start_idx + length(), (uint32_t)source_size);

    channel_buffer.assign(source_ptr + src_start_idx, source_ptr + src_end_idx);
  } catch (std::out_of_range const &err) {
    // TODO: maybe throw here?
    LOGE("Out of range exception: %s\n", err.what());
    return;
  }
}

void AudioBuffer::CopyFromChannel(float *dest_ptr, const uint32_t &dest_size,
                                  const uint32_t channel_number,
                                  const uint32_t &src_start_idx) const {
  // TODO: maybe throw in error?
  if (dest_ptr) {
    try {
      const std::vector<float> &source_vec =
          audio_channel_buffers_[channel_number];
      const uint32_t copy_cnt =
          std::min((uint32_t)source_vec.size() - src_start_idx, dest_size);

      std::memcpy(dest_ptr, &source_vec[src_start_idx],
                  sizeof(source_vec[0]) * copy_cnt);
    } catch (std::out_of_range const &e) {
      LOGE("Out of range exception: %s\n", e.what());
    } catch (std::exception const &e) {
      LOGE("Unknown exception: %s\n", e.what());
    }
  } else {
    LOGE("Invalid dest pointer\n");
  }
}

std::vector<float> AudioBuffer::GetChannelData(const uint32_t channel) const {
  // maybe there is an other better way instead of return copy
  try {
    return audio_channel_buffers_.at(channel);
  } catch (std::out_of_range const &err) {
    LOGE("Channel %u is out of range of %zu\n", channel,
         audio_channel_buffers_.size());
  }
  return {};
}
} // namespace js_audio