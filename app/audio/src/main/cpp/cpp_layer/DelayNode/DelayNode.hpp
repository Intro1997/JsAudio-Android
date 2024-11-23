#pragma once
#include "AudioNode.hpp"
#include "AudioParam.hpp"
#include <memory>

namespace js_audio {
class DelayNode : public AudioNode {
public:
  struct DelayOptions : public AudioNode::AudioNodeOptions {
    float max_delay_time;
    float delay_time;
  };

  static std::shared_ptr<DelayNode>
  CreateDelayNode(const DelayOptions &options, const float &sample_rate,
                  std::shared_ptr<std::mutex> audio_context_lock_ref);
  static DelayOptions GetDefaultOptions();

  std::shared_ptr<AudioParam> delay_time_ref() const;

  float delay_time_value() const;
  void UpdateDelaySampleIdx(const float &delay_sample_idx);
  void ProduceSamples(const size_t &sample_size,
                      std::vector<std::vector<float>> &output) override;
  void ConnectTo(std::shared_ptr<AudioNode> dst_audio_node_ref) override;
  void BeConnectedTo(std::shared_ptr<AudioNode> src_audio_node_ref) override;
  void Disconnect() override;
  void BeDisconnected(const AudioNode &audio_node) override;
  static bool IsValidMaxDelayTime(const float &max_delay_time);

  static uint32_t kNumberOfInputs;
  static uint32_t kNumberOfOutputs;
  static uint32_t kMaxOfMaxDelayTime;
  static uint32_t kMinOfMaxDelayTime;

protected:
  DelayNode(const DelayOptions &options, const float &sample_rate,
            std::shared_ptr<std::mutex> audio_context_lock_ref);

private:
  void set_delay_time_value(const float &delay_time_value);
  void Process(const size_t &sample_size,
                      std::vector<std::vector<float>> &output);
  bool IsBufferValid(const size_t &sample_size,
                     const std::vector<std::vector<float>> &output);
  void MakeBufferInvalid(const size_t channel_size);
  void WriteOutputToBuffer(const size_t &sample_size,
                           const std::vector<std::vector<float>> &output);
  void ReadBufferToOutput(const size_t &sample_size,
                          std::vector<std::vector<float>> &output);

  std::shared_ptr<AudioParam> delay_time_ref_;
  mutable std::mutex delay_time_lock_;
  float delay_time_value_;

  std::vector<std::vector<float>> buffer_;

  std::weak_ptr<AudioNode> dst_audio_node_ptr_;
  std::shared_ptr<AudioNode> src_audio_node_ref_;

  const float sample_rate_;

  uint32_t current_sample_idx_;
  uint32_t delay_sample_idx_;
};
} // namespace js_audio
