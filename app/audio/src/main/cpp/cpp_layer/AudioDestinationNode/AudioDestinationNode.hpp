#pragma once
#include "AudioNode.hpp"

#include <SLES/OpenSLES.h>
#include <memory>
namespace js_audio {
class AudioDestinationNode : public AudioNode {
public:
  AudioDestinationNode(
      const uint32_t &channel_count,
      std::shared_ptr<std::mutex> audio_context_lock_ref,
      const uint32_t &number_of_inputs = 1,
      const uint32_t &number_of_outputs = 0,
      const ChannelCountMode &channel_count_mode = ChannelCountMode::kExplicit,
      const ChannelInterpretation &channel_interpretation =
          ChannelInterpretation::kSpeakers);

  void ProduceSamples(size_t sample_size,
                      std::vector<std::vector<float>> &output) override;

  void ConnectTo(std::shared_ptr<AudioNode> dst_audio_node_ref) override;
  void BeConnectedTo(std::shared_ptr<AudioNode> src_audio_node_ref) override;
  void Disconnect() override;
  void BeDisconnected(const AudioNode &audio_node) override;

  uint32_t max_channel_count() const;

  void SetSourceAudioNode(std::shared_ptr<AudioNode> src_node_ref);

private:
  void GetSourceNodesOutput(float *data, uint32_t sample_size);

  const uint32_t max_channel_count_;
  std::shared_ptr<AudioNode> src_audio_node_ref_;
};
} // namespace js_audio
