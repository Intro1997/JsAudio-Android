#pragma once
#include "AudioDestinationNode.hpp"
#include "BaseAudioContext.hpp"
namespace js_audio {
class AudioContext : public BaseAudioContext{
public:
  AudioContext();

  /**
   * produce sample_size count samples
   * data: the pointer to receive produced samples
   * sample_size: size of samples to be produced
   */
  void ProduceSamples(SLint16 *data, uint32_t sample_size);
};
} // namespace js_audio
