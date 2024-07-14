#include "AudioNode.hpp"

namespace js_audio {

uint32_t AudioNode::number_of_inputs() const { return number_of_inputs_; }
uint32_t AudioNode::number_of_outputs() const { return number_of_outputs_; }
uint32_t AudioNode::channel_count() const { return channel_count_; }
std::string AudioNode::channel_count_mode() const {
  return channel_count_mode_;
}

void AudioNode::set_number_of_inputs(const uint32_t &number_of_inputs) {
  number_of_inputs_ = number_of_inputs;
}
void AudioNode::set_number_of_outputs(const uint32_t &number_of_outputs) {
  number_of_outputs_ = number_of_outputs;
}
void AudioNode::set_channel_count(const uint32_t &channel_count) {
  channel_count_ = channel_count;
}
void AudioNode::set_channel_count_mode(const std::string &channel_count_mode) {
  channel_count_mode_ = channel_count_mode;
}
} // namespace js_audio
