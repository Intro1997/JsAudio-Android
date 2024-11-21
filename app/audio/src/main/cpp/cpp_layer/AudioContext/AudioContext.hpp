#pragma once
#include "AudioDestinationNode.hpp"
#include "BaseAudioContext.hpp"
namespace js_audio {
class AudioContext : public BaseAudioContext {
public:
  AudioContext();

  double current_time() override;
  double GetCurrentTime() override;

private:
  const double construct_microsecond_time_;
};
} // namespace js_audio
