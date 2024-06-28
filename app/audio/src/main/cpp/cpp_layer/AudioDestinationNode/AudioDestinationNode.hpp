#pragma once
#include <SLES/OpenSLES.h>
#include <memory>
class AudioDestinationNode {
public:
  AudioDestinationNode();

  /**
   * produce sample_size count samples
   * data: the pointer to receive produced samples
   * sample_size: size of samples to be produced
   */
  void ProduceSamples(float *data, uint32_t sample_size);

private:
  void GetSourceNodesOutput(float *data, uint32_t sample_size);
};