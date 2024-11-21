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
      std::shared_ptr<std::mutex> audio_context_lock_ref);
  void ConnectTo(std::shared_ptr<AudioNode> dst_audio_node_ref) override;
  void BeConnectedTo(std::shared_ptr<AudioNode> src_audio_node_ref) override;
  void Disconnect() override;
  void BeDisconnected(const AudioNode &audio_node) override;
  void Start(const double &when);
  void Stop(const double &when);

  State state() const;
  void set_state(const State &state);
  bool has_started() const;
  void set_has_started(const bool &value);

protected:
  bool has_started_;

  double start_time() const;
  void set_start_time(const double &time);
  double stop_time() const;
  void set_stop_time(const double &time);

  double start_time_;
  double stop_time_;
  mutable std::mutex start_time_lock_;
  mutable std::mutex stop_time_lock_;

private:
  std::weak_ptr<AudioNode> dst_audio_node_ptr_;
  mutable std::mutex state_lock_;
  std::shared_ptr<State> state_ref_;
};
} // namespace js_audio
