#pragma once
#include "AudioNode.hpp"

#include <SLES/OpenSLES.h>
#include <memory>
namespace js_audio {
class AudioDestinationNode : public AudioNode {
public:
  AudioDestinationNode() = default;

  /**
   * produce sample_size count samples
   * data: the pointer to receive produced samples
   * sample_size: size of samples to be produced
   */
  void ProduceSamples(float *data, uint32_t sample_size);

  uint32_t max_channel_count() const;
  void set_max_channel_count(const uint32_t &max_channel_count);

private:
  void GetSourceNodesOutput(float *data, uint32_t sample_size);

  uint32_t max_channel_count_;
};
} // namespace js_audio
