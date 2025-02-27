#pragma once

#include "SLES/OpenSLES_Android.h"
#include <vector>

/**
 * One sample has three property
 * - element size, such as SLint16
 * - sample size, such as we have 1000 samples
 *
 * AudioMixer can mix two sample array to on sample array in following
 * situation, if we have two sample array sample_array0 and sample_array1:
 * 1. sample_array0.element_size >= sample_array1.element_size or
 * sample_array0.element_size < sample_array1.element_size
 * 2. sample_array0.sample >= sample_array1.sample or sample_array0.sample <
 * sample_array1.sample
 */
namespace js_audio {
class AudioMixer {
public:
  static void
  Convert2ChannelFloatToSlint16(const std::vector<std::vector<float>> &src,
                                std::vector<SLint16> &dst);
  static void MixSample(const std::vector<SLint16> &samples0,
                        const std::vector<SLint16> &samples1,
                        std::vector<SLint16> &output);
  static void MixSample(const std::vector<float> &samples0,
                        const std::vector<float> &samples1,
                        std::vector<float> &output);
  static void MixBuffer(const std::vector<std::vector<float>> &left_b,
                        const std::vector<std::vector<float>> &right_b,
                        std::vector<std::vector<float>> &output_b);
};
} // namespace js_audio
