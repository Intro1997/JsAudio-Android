#pragma once
#include "AudioNode.hpp"
#include <mutex>

namespace js_audio {
class AudioScheduledSourceNode : public AudioNode {
protected:
  enum class State { Start, Stop };

public:
  AudioScheduledSourceNode(
      const uint32_t &number_of_inputs, const uint32_t &number_of_outputs,
      const uint32_t &channel_count,
      const AudioNode::ChannelCountMode &channel_count_mode,
      const AudioNode::ChannelInterpretation &channel_interpretation,
      std::shared_ptr<std::mutex> audio_context_lock);
  void ConnectTo(std::shared_ptr<AudioNode> dst_audio_node_ptr) override;
  void BeConnectedTo(std::shared_ptr<AudioNode> src_audio_node_ptr) override;
  void Disconnect() override;
  void BeDisconnected(const AudioNode &audio_node) override;
  void Start(const float &when);
  void Stop(const float &when);

  State state() const;
  void set_state(const State &state);

private:
  void ScheduleStateChange(const State &state, const float &delay_time);

  std::weak_ptr<AudioNode> dst_audio_node_ptr_;
  mutable std::mutex state_lock_;
  std::shared_ptr<State> state_ref_;
  bool has_started_;
};
} // namespace js_audio
