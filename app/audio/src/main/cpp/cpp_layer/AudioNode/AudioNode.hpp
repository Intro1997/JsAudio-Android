#pragma once
#include <SLES/OpenSLES_Android.h>
#include <shared_mutex>
#include <string>
#include <vector>

namespace js_audio {

class AudioNode {
public:
  AudioNode(const uint32_t &number_of_inputs, const uint32_t &number_of_outputs,
            const uint32_t &channel_count,
            const std::string &channel_count_mode,
            const std::string &channel_interpretation,
            std::shared_ptr<std::mutex> audio_context_lock);

  uint32_t number_of_inputs() const;
  uint32_t number_of_outputs() const;
  uint32_t channel_count() const;
  std::string channel_count_mode() const;
  std::string channel_interpretation() const;

  virtual void ProduceSamples(size_t sample_size,
                              std::vector<std::vector<float>> &output);
  virtual void ConnectTo(std::shared_ptr<AudioNode> dst_audio_node_ptr) = 0;
  virtual void BeConnectedTo(std::shared_ptr<AudioNode> src_audio_node_ptr) = 0;
  virtual void Disconnect() = 0;
  virtual void BeDisconnected(const AudioNode &audio_node) = 0;

protected:
  bool IsSelfPtr(std::shared_ptr<AudioNode> other);
  void FillWithZeros(size_t sample_size,
                     std::vector<std::vector<float>> &output);

  const uint32_t number_of_inputs_;
  const uint32_t number_of_outputs_;
  const uint32_t channel_count_;
  const std::string channel_count_mode_;
  const std::string channel_interpretation_;
  std::shared_ptr<std::mutex> audio_context_lock_;
};
} // namespace js_audio