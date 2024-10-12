#pragma once
#include "AudioNode.hpp"

namespace js_audio {
class AudioScheduledSourceNode : public AudioNode {
public:
  AudioScheduledSourceNode(const uint32_t &number_of_inputs,
                           const uint32_t &number_of_outputs,
                           const uint32_t &channel_count,
                           const std::string &channel_count_mode,
                           const std::string &channel_interpretation,
                           std::shared_ptr<std::mutex> audio_context_lock);
  void ConnectTo(std::shared_ptr<AudioNode> dst_audio_node_ptr) override;
  void BeConnectedTo(std::shared_ptr<AudioNode> src_audio_node_ptr) override;
  void Disconnect() override;
  void BeDisconnected(const AudioNode &audio_node) override;

private:
  std::weak_ptr<AudioNode> dst_audio_node_ptr_;
};
} // namespace js_audio
