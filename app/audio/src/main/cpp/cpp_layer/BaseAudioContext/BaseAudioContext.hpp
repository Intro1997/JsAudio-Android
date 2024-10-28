#pragma once
#include "AudioDestinationNode.hpp"
#include <tuple>
namespace js_audio {
class BaseAudioContext {
public:
  enum class ContextType { kOnline = 0, kOffline };

  BaseAudioContext(const ContextType &type = ContextType::kOffline,
                   const uint32_t &number_of_channels = 0,
                   const float &sample_rate = 0.0f);
  BaseAudioContext(const std::tuple<ContextType, uint32_t, float> &params);

  float sample_rate() const;
  uint32_t number_of_channels() const;

  virtual double GetCurrentTime();
  std::shared_ptr<std::mutex> GetLock() const;

  void ProduceSamples(size_t sample_size,
                      std::vector<std::vector<float>> &output);

  bool IsOnlineContext() const;

  std::shared_ptr<AudioDestinationNode> audio_destination_node_ptr();

private:
  std::shared_ptr<std::mutex> audio_context_lock_;

protected:
  // be assigned in derived class
  float sample_rate_;
  uint32_t number_of_channels_;
  ContextType context_type_;

  std::shared_ptr<AudioDestinationNode> audio_destination_node_ptr_;
};
} // namespace js_audio