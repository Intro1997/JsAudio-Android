#include "AudioMixer.hpp"
#include "logger.hpp"
namespace js_audio {
static inline SLint32 AddSample(const SLint16 &left, const SLint16 &right) {
  SLint32 left_copy = left;
  SLint32 right_copy = right;
  return left_copy + right_copy;
}

// TODO: MixSample only support 2 double channel sample now
void AudioMixer::MixSample(const std::vector<SLint16> &samples0,
                           const std::vector<SLint16> &samples1,
                           std::vector<SLint16> &output) {
  size_t output_size = std::min(samples0.size(), samples1.size());
  for (size_t i = 0; i < output_size; i++) {
    output[i] = AddSample(samples0[i], samples1[i]) * 0.5;
  }
}

void AudioMixer::Convert2ChannelFloatToSlint16(
    const std::vector<std::vector<float>> &src, std::vector<SLint16> &dst) {
  if (src.size() != 2) {
    LOGE("Only process 2 channel here!\n");
    return;
  }

  const size_t convert_size = std::min(src[0].size(), src[1].size());
  dst.clear();
  dst.resize(convert_size * 2);
  for (size_t i = 0; i < convert_size; i++) {
    dst[i * 2] = std::clamp(src[0][i], -1.0f, 1.0f) * (float)INT16_MAX;
    dst[i * 2 + 1] = std::clamp(src[1][i], -1.0f, 1.0f) * (float)INT16_MAX;
  }
}

} // namespace js_audio
