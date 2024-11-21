#pragma once
#include "AudioDestinationNode.hpp"
#include <tuple>
namespace js_audio {
class BaseAudioContext {
public:
  friend class AudioDestinationNode;
  enum class ContextType { kOnline = 0, kOffline };

  BaseAudioContext(const ContextType &type = ContextType::kOffline,
                   const uint32_t &number_of_channels = 0,
                   const float &sample_rate = 0.0f);
  BaseAudioContext(const std::tuple<ContextType, uint32_t, float> &params);

  float sample_rate() const;
  uint32_t number_of_channels() const;

  virtual double current_time();
  virtual double GetCurrentTime();

  std::shared_ptr<std::mutex> GetLock() const;

  void ProduceSamples(const size_t &sample_size,
                      std::vector<std::vector<float>> &output);

  bool IsOnlineContext() const;

  std::shared_ptr<AudioDestinationNode> audio_destination_node_ref();

private:
  std::shared_ptr<std::mutex> audio_context_lock_ref_;

protected:
  virtual void UpdateCurrentTime(const size_t &sample_size);

  // be assigned in derived class
  double current_time_;
  std::mutex current_time_lock_;

  float sample_rate_;
  uint32_t number_of_channels_;
  ContextType context_type_;

  std::shared_ptr<AudioDestinationNode> audio_destination_node_ref_;
};
} // namespace js_audio