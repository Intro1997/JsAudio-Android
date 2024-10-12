#pragma once
#include "AudioBuffer.hpp"
#include "AudioDestinationNode.hpp"
#include "BaseAudioContext.hpp"
#include <thread>
namespace js_audio {
class OfflineAudioContext : public BaseAudioContext {
public:
  OfflineAudioContext(const uint32_t &number_of_channels = 0,
                      const uint32_t &length = 0,
                      const float &sample_rate = 0.0f);

  // TODO: alias a type ot cb
  bool
  StartRendering(const std::function<void(std::shared_ptr<AudioBuffer>)> &cb);
  const uint32_t &length() const;

private:
  enum class RenderState { kRunning = 0, kFinish };

  // TODO: alias a type ot cb
  void
  InnerRendering(const std::function<void(std::shared_ptr<AudioBuffer>)> &cb);

  RenderState render_state() const;
  void set_render_state(RenderState state);

  uint32_t length_;

  std::thread offline_rendering_thread_;
  RenderState render_state_;
  mutable std::mutex render_state_lock_;
};
} // namespace js_audio
