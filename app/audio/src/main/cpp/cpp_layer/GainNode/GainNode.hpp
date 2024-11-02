#pragma once
#include "AudioNode.hpp"
#include "AudioParam.hpp"
#include <memory>

namespace js_audio {
class GainNode : public AudioNode {
public:
  struct GainNodeOptions : public AudioNode::AudioNodeOptions {
    float gain;
  };

public:
  static std::shared_ptr<GainNode>
  CreateGainNode(std::shared_ptr<std::mutex> audio_context_lock_ref);
  static std::shared_ptr<GainNode>
  CreateGainNode(const GainNodeOptions &options,
                 std::shared_ptr<std::mutex> audio_context_lock_ref);
  static GainNodeOptions GetDefaultOptions();

  std::shared_ptr<AudioParam> gain_ref() const;

  void ConnectTo(std::shared_ptr<AudioNode> dst_audio_node_ref) override;
  void BeConnectedTo(std::shared_ptr<AudioNode> src_audio_node_ref) override;
  void Disconnect() override;
  void BeDisconnected(const AudioNode &audio_node) override;
  void ProduceSamples(const size_t &sample_size,
                      std::vector<std::vector<float>> &output) override;

protected:
  GainNode(const GainNodeOptions &options,
           std::shared_ptr<std::mutex> audio_context_lock_ref);
  void set_gain_value(const float &gain_value);

private:
  void ProcessSamples(const size_t &sample_size,
                      std::vector<std::vector<float>> &outputs);

  std::shared_ptr<AudioParam> gain_ref_;
  std::weak_ptr<AudioNode> dst_audio_node_ptr_;
  std::shared_ptr<AudioNode> src_audio_node_ref_;

  float gain_value_;

  static const uint32_t kNumberOfInputs;
  static const uint32_t kNumberOfOutputs;
};

} // namespace js_audio
