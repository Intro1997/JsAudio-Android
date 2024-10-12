#pragma once
#include "AudioDestinationNode.hpp"
#include <tuple>
namespace js_audio {
class BaseAudioContext {
public:
  BaseAudioContext(const uint32_t &number_of_channels = 0,
                   const float &sample_rate = 0.0f);
  BaseAudioContext(const std::tuple<uint32_t, float> &params);

  float sample_rate() const;
  uint32_t number_of_channels() const;

  double GetCurrentTime();
  std::shared_ptr<std::mutex> GetLock() const;

  void ProduceSamples(size_t sample_cnt, std::vector<SLint16> &output);

  std::shared_ptr<AudioDestinationNode> audio_destination_node_ptr();

private:
  const double construct_microsecond_time_;
  std::shared_ptr<std::mutex> audio_context_lock_;

protected:
  // be assigned in derived class
  float sample_rate_;
  uint32_t number_of_channels_;

  float set_sample_rate(const float &sample_rate);
  double
  set_construct_millisecond_time_(const double &construct_millisecond_time_);
  std::shared_ptr<AudioDestinationNode> audio_destination_node_ptr_;
};
} // namespace js_audio