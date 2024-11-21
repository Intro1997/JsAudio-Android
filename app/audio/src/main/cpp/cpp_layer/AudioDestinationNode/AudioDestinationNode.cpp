#include "AudioDestinationNode.hpp"
#include "AudioMixer.hpp"
#include "BaseAudioContext.hpp"
#include "ChannelMixer.hpp"
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
  if (src_audio_node_refs_.empty()) {
    FillWithZeros(sample_size, output);
    return;
  }
  std::vector<std::vector<float>> temp_output(output.size());
  FillWithZeros(sample_size, temp_output);

  auto &refs = src_audio_node_refs_;
  for (size_t i = 0; i < refs.size(); i++) {
    auto ref = refs[i];
    if (ref) {
      ref->ProduceSamples(sample_size, temp_output);
      temp_output = ChannelMixer::DoMix(temp_output, channel_interpretation_,
                                        channel_count_);
      if (i != 0) {
        AudioMixer::MixBuffer(temp_output, output, output);
      } else {
        output = temp_output;
      }
    }
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
  auto &refs = src_audio_node_refs_;
  auto condition_func = [&](std::shared_ptr<AudioNode> curr) {
    return curr.get() == src_audio_node_ref.get();
  };
  if (std::find_if(refs.begin(), refs.end(), condition_func) == refs.end()) {
    refs.push_back(src_audio_node_ref);
  }
}

void AudioDestinationNode::Disconnect() { LOGE("Shouldn't get in here!\n"); }

void AudioDestinationNode::BeDisconnected(const AudioNode &audio_node) {
  auto &refs = src_audio_node_refs_;
  auto ref = std::find_if(refs.begin(), refs.end(),
                          [&](std::shared_ptr<AudioNode> curr) -> bool {
                            return curr.get() == &audio_node;
                          });
  if (ref != refs.begin()) {
    refs.erase(ref);
  } else {
    LOGE("BeDisconnected failed! Parameter AudioNode is different from "
         "current source AudioNode.\n");
  }
}

uint32_t AudioDestinationNode::max_channel_count() const {
  return max_channel_count_;
}

void AudioDestinationNode::SetSourceAudioNode(
    std::shared_ptr<AudioNode> src_node_ptr) {}
}; // namespace js_audio