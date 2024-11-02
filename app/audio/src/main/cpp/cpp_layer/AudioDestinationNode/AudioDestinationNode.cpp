#include "AudioDestinationNode.hpp"
#include "BaseAudioContext.hpp"
#include "WaveProducer.hpp"
#include "logger.hpp"

namespace js_audio {

AudioDestinationNode::AudioDestinationNode(
    const uint32_t &channel_count,
    std::shared_ptr<std::mutex> audio_context_lock_ref,
    const uint32_t &number_of_inputs, const uint32_t &number_of_outputs,
    const ChannelCountMode &channel_count_mode,
    const ChannelInterpretation &channel_interpretation)
    : AudioNode(number_of_inputs, number_of_outputs, channel_count,
                channel_count_mode, channel_interpretation,
                audio_context_lock_ref),
      max_channel_count_(channel_count) {}

void AudioDestinationNode::ProduceSamples(
    const size_t &sample_size, std::vector<std::vector<float>> &output) {
  if (src_audio_node_ref_) {
    src_audio_node_ref_->ProduceSamples(sample_size, output);
  } else {
    FillWithZeros(sample_size, output);
  }
}

void AudioDestinationNode::ConnectTo(
    std::shared_ptr<AudioNode> dst_shared_ref) {
  LOGE("Shouldn't get in here!\n");
}

void AudioDestinationNode::BeConnectedTo(
    std::shared_ptr<AudioNode> src_audio_node_ref) {
  if (IsSelfPtr(src_audio_node_ref)) {
    LOGE("Self cannot be connected to self!\n");
    return;
  }

  std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
  src_audio_node_ref_ = src_audio_node_ref;
}

void AudioDestinationNode::Disconnect() { LOGE("Shouldn't get in here!\n"); }

void AudioDestinationNode::BeDisconnected(const AudioNode &audio_node) {
  if (&audio_node != src_audio_node_ref_.get()) {
    LOGE("BeDisconnected failed! Parameter AudioNode is different from "
         "current "
         "source AudioNode.\n");
    return;
  }

  std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
  src_audio_node_ref_.reset();
}

uint32_t AudioDestinationNode::max_channel_count() const {
  return max_channel_count_;
}

void AudioDestinationNode::SetSourceAudioNode(
    std::shared_ptr<AudioNode> src_node_ptr) {}
}; // namespace js_audio