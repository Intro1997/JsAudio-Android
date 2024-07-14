#pragma once
#include "AudioDestinationNode.hpp"
namespace js_audio {
class BaseAudioContext {
public:
  BaseAudioContext();

  float sample_rate() const;

  double GetCurrentTime();

  std::shared_ptr<AudioDestinationNode> audio_destination_node_ptr();

protected:
  float set_sample_rate(const float &sample_rate);
  double
  set_construct_millisecond_time_(const double &construct_millisecond_time_);
  std::shared_ptr<AudioDestinationNode> audio_destination_node_ptr_;

private:
  float sample_rate_; // unit in hz
  const double construct_microsecond_time_;
};
} // namespace js_audio