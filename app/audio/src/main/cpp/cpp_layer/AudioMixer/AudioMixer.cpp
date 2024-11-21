#include "AudioMixer.hpp"
#include "logger.hpp"
namespace js_audio {
static inline SLint32 AddSample(const SLint16 &left, const SLint16 &right) {
  SLint32 left_copy = left;
  SLint32 right_copy = right;
  SLint32 res = left_copy + right_copy;
  SLint32 sign = res > 0 ? 1 : -1;
  return std::abs(res) > INT16_MAX ? sign * INT16_MAX : res;
}

static inline float AddSample(const float &left, const float &right) {
  double copy_left = left;
  double copy_right = right;
  double res = copy_left + copy_right;
  double sign = res > 0 ? 1 : -1;
  return std::abs(res) > FLT_MAX ? sign * FLT_MAX : res;
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

// TODO: MixSample only support 2 double channel sample now
void AudioMixer::MixSample(const std::vector<SLint16> &samples0,
                           const std::vector<SLint16> &samples1,
                           std::vector<SLint16> &output) {
  size_t output_size = std::min(samples0.size(), samples1.size());
  if (output.size() < output_size &&
      (&output != &samples0 || &output != &samples1)) {
    output.resize(output_size);
  }
  for (size_t i = 0; i < output_size; i++) {
    output[i] = AddSample(samples0[i], samples1[i]) * 0.5;
  }
}
void AudioMixer::MixSample(const std::vector<float> &samples0,
                           const std::vector<float> &samples1,
                           std::vector<float> &output) {
  size_t output_size = std::min(samples0.size(), samples1.size());
  if (output.size() < output_size &&
      (&output != &samples0 || &output != &samples1)) {
    output.resize(output_size);
  }
  for (size_t i = 0; i < output_size; i++) {
    output[i] = AddSample(samples0[i], samples1[i]);
  }
}

void AudioMixer::MixBuffer(const std::vector<std::vector<float>> &left_b,
                           const std::vector<std::vector<float>> &right_b,
                           std::vector<std::vector<float>> &output_b) {
  size_t channel_size = std::min(left_b.size(), right_b.size());
  if (output_b.size() < channel_size &&
      (&output_b != &left_b || &output_b != &right_b)) {
    output_b.resize(channel_size);
  }
  for (size_t c = 0; c < channel_size; c++) {
    MixSample(left_b[c], right_b[c], output_b[c]);
  }
}

} // namespace js_audio
