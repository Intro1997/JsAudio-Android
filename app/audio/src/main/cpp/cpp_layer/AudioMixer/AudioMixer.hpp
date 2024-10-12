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
  static void MixSample(const std::vector<SLint16> &samples0,
                        const std::vector<SLint16> &samples1,
                        std::vector<SLint16> &output);
};
} // namespace js_audio
