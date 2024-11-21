#include "AudioBuffer.hpp"
#include "logger.hpp"
#include <cstdint>
namespace js_audio {
AudioBuffer::AudioBuffer(const uint32_t &number_of_channel,
                         const uint32_t &length, const float &sample_rate)
    : number_of_channel_(number_of_channel), length_(length),
      sample_rate_(sample_rate) {
  if (sample_rate_ == 0) {
    duration_ = 0;
  } else {
    duration_ = ((double)length_ / (double)sample_rate_);
  }
  audio_channel_buffers_.resize(number_of_channel_);
  for (auto &channel_buffer : audio_channel_buffers_) {
    channel_buffer.resize(length_, 0);
  }
}

AudioBuffer::AudioBuffer(std::vector<std::vector<float>> &&src)
    : number_of_channel_(src.size()), audio_channel_buffers_(std::move(src)) {
  if (audio_channel_buffers_.size()) {
    size_t min_len = audio_channel_buffers_[0].size();
    for (const auto &channel : audio_channel_buffers_) {
      min_len = std::min(min_len, channel.size());
    }
    length_ = min_len;
  }
}

uint32_t AudioBuffer::number_of_channel() const { return number_of_channel_; }

uint32_t AudioBuffer::length() const { return length_; }

float AudioBuffer::sample_rate() const { return sample_rate_; }

double AudioBuffer::duration() const { return duration_; }

void AudioBuffer::CopyToChannel(const float *source_ptr,
                                const uint32_t &source_size,
                                const uint32_t channel_number,
                                const uint32_t &dst_start_idx) {
  if (source_ptr == nullptr) {
    LOGE("Invalid source data pointer\n");
    return;
  }
  if (dst_start_idx >= length_) {
    LOGE("Source start index(%u) is larger than channel data size(%u)!\n",
         dst_start_idx, length_);
    return;
  }
  if (channel_number >= number_of_channel_) {
    LOGE("Request channel number(%u) is out of channel range[0, %u)!\n",
         channel_number, number_of_channel_);
    return;
  }
  try {
    std::vector<float> &channel_buffer =
        audio_channel_buffers_.at(channel_number);

    std::copy(source_ptr, source_ptr + source_size,
              channel_buffer.begin() + dst_start_idx);
  } catch (std::exception err) {
    LOGE("Unknown exception: %s\n", err.what());
    return;
  }
}

void AudioBuffer::CopyFromChannel(float *dest_ptr, const uint32_t &dest_size,
                                  const uint32_t channel_number,
                                  const uint32_t &src_start_idx) const {
  if (dest_ptr) {
    if (channel_number >= number_of_channel_) {
      LOGE("channel number(%u) is large than number of channel(%u).\n",
           channel_number, number_of_channel_);
      return;
    }
    try {
      const std::vector<float> &source_vec =
          audio_channel_buffers_[channel_number];

      if (src_start_idx >= source_vec.size()) {
        LOGE("start index(%u) is large than length(%zu).\n", src_start_idx,
             source_vec.size());
        return;
      }

      const uint32_t copy_cnt =
          std::min((uint32_t)source_vec.size() - src_start_idx, dest_size);

      std::memcpy(dest_ptr, &source_vec[src_start_idx],
                  sizeof(source_vec[0]) * copy_cnt);
    } catch (std::exception const &e) {
      LOGE("Unknown exception: %s\n", e.what());
    }
  } else {
    LOGE("Invalid dest pointer\n");
  }
}

const std::vector<float> *
AudioBuffer::GetChannelDataPtr(const uint32_t channel) const {
  // maybe there is an other better way instead of return copy
  try {
    return &audio_channel_buffers_.at(channel);
  } catch (std::out_of_range const &err) {
    LOGE("Channel %u is out of range of %zu\n", channel,
         audio_channel_buffers_.size());
  }
  return nullptr;
}
} // namespace js_audio