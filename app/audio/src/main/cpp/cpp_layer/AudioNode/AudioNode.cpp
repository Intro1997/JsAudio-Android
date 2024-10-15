#include "AudioNode.hpp"
#include "BaseAudioContext.hpp"
#include "WaveProducer.hpp"
#include "logger.hpp"

namespace js_audio {

AudioNode::AudioNode(const uint32_t &number_of_inputs,
                     const uint32_t &number_of_outputs,
                     const uint32_t &channel_count,
                     const std::string &channel_count_mode,
                     const std::string &channel_interpretation,
                     std::shared_ptr<std::mutex> audio_context_lock)
    : number_of_inputs_(number_of_inputs),
      number_of_outputs_(number_of_outputs), channel_count_(channel_count),
      channel_count_mode_(channel_count_mode),
      channel_interpretation_(channel_interpretation),
      audio_context_lock_(audio_context_lock) {}

uint32_t AudioNode::number_of_inputs() const { return number_of_inputs_; }
uint32_t AudioNode::number_of_outputs() const { return number_of_outputs_; }
uint32_t AudioNode::channel_count() const { return channel_count_; }
std::string AudioNode::channel_count_mode() const {
  return channel_count_mode_;
}
std::string AudioNode::channel_interpretation() const {
  return channel_interpretation_;
}

void AudioNode::ProduceSamples(size_t sample_size,
                               std::vector<std::vector<float>> &output) {
  FillWithZeros(sample_size, output);
}

void AudioNode::FillWithZeros(size_t sample_size,
                              std::vector<std::vector<float>> &output) {
  output.resize(channel_count_);
  for (auto &channel : output) {
    channel.clear();
    channel.resize(sample_size, 0);
  }
}

bool AudioNode::IsSelfPtr(std::shared_ptr<AudioNode> other) {
  return other.get() == this;
}

} // namespace js_audio
