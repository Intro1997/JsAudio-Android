#include "AudioNode.hpp"
#include "BaseAudioContext.hpp"
#include "WaveProducer.hpp"
#include "logger.hpp"

namespace js_audio {

const uint32_t AudioNode::kMinChannelCount = 1;
const uint32_t AudioNode::kMaxChannelCount = 32;

AudioNode::AudioNode(const uint32_t &number_of_inputs,
                     const uint32_t &number_of_outputs,
                     const uint32_t &channel_count,
                     const ChannelCountMode &channel_count_mode,
                     const ChannelInterpretation &channel_interpretation,
                     std::shared_ptr<std::mutex> audio_context_lock_ref)
    : number_of_inputs_(number_of_inputs),
      number_of_outputs_(number_of_outputs), channel_count_(channel_count),
      channel_count_mode_(channel_count_mode),
      channel_interpretation_(channel_interpretation),
      audio_context_lock_ref_(audio_context_lock_ref) {}

AudioNode::AudioNode(const uint32_t &number_of_inputs,
                     const uint32_t &number_of_outputs,
                     const AudioNodeOptions &options,
                     std::shared_ptr<std::mutex> audio_context_lock_ref)
    : AudioNode(number_of_inputs, number_of_outputs, options.channel_count,
                options.channel_count_mode, options.channel_interpretation,
                audio_context_lock_ref) {}

bool AudioNode::IsValidChannelCount(const uint32_t &channel_count) {
  if (channel_count < kMinChannelCount || channel_count > kMaxChannelCount) {
    return false;
  }
  return true;
}

bool AudioNode::ConvertToChannelCountMode(
    const std::string &str_channel_count_mode,
    ChannelCountMode &channel_count_mode) {
  if (str_channel_count_mode == "clamped-max") {
    channel_count_mode = ChannelCountMode::kClampedMax;
    return true;
  } else if (str_channel_count_mode == "explicit") {
    channel_count_mode = ChannelCountMode::kExplicit;
    return true;
  } else if (str_channel_count_mode == "max") {
    channel_count_mode = ChannelCountMode::kMax;
    return true;
  }
  return false;
}

bool AudioNode::ConvertToChannelInterpretation(
    const std::string &str_channel_interpretation,
    ChannelInterpretation &channel_interpretation) {
  if (str_channel_interpretation == "discrete") {
    channel_interpretation = ChannelInterpretation::kDiscrete;
    return true;
  } else if (str_channel_interpretation == "speakers") {
    channel_interpretation = ChannelInterpretation::kSpeakers;
    return true;
  }
  return false;
}

std::string AudioNode::ConvertChannelCountModeToString(
    const ChannelCountMode &channel_count_mode) {
  if (channel_count_mode == ChannelCountMode::kClampedMax) {
    return "clamped-max";
  } else if (channel_count_mode == ChannelCountMode::kExplicit) {
    return "explicit";
  } else if (channel_count_mode == ChannelCountMode::kMax) {
    return "max";
  }
  return "";
}

std::string AudioNode::ConvertChannelInterpretationToString(
    const ChannelInterpretation &channel_interpretation) {
  if (channel_interpretation == ChannelInterpretation::kDiscrete) {
    return "discrete";
  } else if (channel_interpretation == ChannelInterpretation::kSpeakers) {
    return "speakers";
  }
  return "";
}

uint32_t AudioNode::number_of_inputs() const { return number_of_inputs_; }

uint32_t AudioNode::number_of_outputs() const { return number_of_outputs_; }

uint32_t AudioNode::channel_count() const { return channel_count_; }

AudioNode::ChannelCountMode AudioNode::channel_count_mode() const {
  return channel_count_mode_;
}

AudioNode::ChannelInterpretation AudioNode::channel_interpretation() const {
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

bool AudioNode::IsSelfPtr(std::shared_ptr<AudioNode> other_ref) {
  return other_ref.get() == this;
}

} // namespace js_audio
