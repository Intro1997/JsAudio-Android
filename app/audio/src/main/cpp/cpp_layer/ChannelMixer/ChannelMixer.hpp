#pragma once
#include "AudioNode.hpp"
namespace js_audio {
class ChannelMixer {
public:
  ChannelMixer() = delete;
  static std::vector<std::vector<float>>
  DoMix(const std::vector<std::vector<float>> &buffer,
        const AudioNode::ChannelInterpretation &interpretation,
        const size_t &aim_channel_size);
};
} // namespace js_audio