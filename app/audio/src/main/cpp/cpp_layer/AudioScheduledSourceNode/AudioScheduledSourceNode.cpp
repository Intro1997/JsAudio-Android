#include "AudioScheduledSourceNode.hpp"
#include "logger.hpp"
namespace js_audio {
AudioScheduledSourceNode::AudioScheduledSourceNode(
    const uint32_t &number_of_inputs, const uint32_t &number_of_outputs,
    const uint32_t &channel_count,
    const AudioNode::ChannelCountMode &channel_count_mode,
    const AudioNode::ChannelInterpretation &channel_interpretation,
    std::shared_ptr<std::mutex> audio_context_lock)
    : AudioNode(number_of_inputs, number_of_outputs, channel_count,
                channel_count_mode, channel_interpretation,
                audio_context_lock) {}

void AudioScheduledSourceNode::ConnectTo(
    std::shared_ptr<AudioNode> dst_audio_node_ptr) {
  if (IsSelfPtr(dst_audio_node_ptr)) {
    LOGE("Error! Cannot connect to self!\n");
    return;
  }

  std::lock_guard<std::mutex> guard(*audio_context_lock_);
  dst_audio_node_ptr_ = dst_audio_node_ptr;
}

void AudioScheduledSourceNode::BeConnectedTo(
    std::shared_ptr<AudioNode> src_audio_node_ptr) {
  LOGE("Error! Should not get in here!\n");
}

void AudioScheduledSourceNode::Disconnect() {
  if (auto dst_audio_node_ref = dst_audio_node_ptr_.lock()) {
    dst_audio_node_ref->BeDisconnected(*this);
  }
  std::lock_guard<std::mutex> guard(*audio_context_lock_);
  dst_audio_node_ptr_.reset();
}

void AudioScheduledSourceNode::BeDisconnected(const AudioNode &audio_node) {
  LOGE("Error! Should not get in here!\n");
}
}; // namespace js_audio