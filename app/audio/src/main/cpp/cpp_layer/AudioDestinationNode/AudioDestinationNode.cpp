#include "AudioDestinationNode.hpp"
namespace js_audio {
uint32_t AudioDestinationNode::max_channel_count() const {
  return max_channel_count_;
}
void AudioDestinationNode::set_max_channel_count(
    const uint32_t &max_channel_count) {
  max_channel_count_ = max_channel_count;
}
}; // namespace js_audio