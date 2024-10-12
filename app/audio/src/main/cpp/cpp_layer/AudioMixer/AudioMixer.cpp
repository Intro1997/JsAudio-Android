#include "AudioMixer.hpp"
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
} // namespace js_audio
