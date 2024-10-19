#pragma once
#include <SLES/OpenSLES_Android.h>
#include <shared_mutex>
#include <string>
#include <vector>

namespace js_audio {

class AudioNode {
public:
  enum class ChannelCountMode { kClampedMax = 0, kExplicit, kMax };
  enum class ChannelInterpretation { kDiscrete = 0, kSpeakers };

  struct AudioNodeOptions {
    uint32_t channel_count;
    ChannelCountMode channel_count_mode;
    ChannelInterpretation channel_interpretation;
  };

  AudioNode(const uint32_t &number_of_inputs, const uint32_t &number_of_outputs,
            const uint32_t &channel_count,
            const ChannelCountMode &channel_count_mode,
            const ChannelInterpretation &channel_interpretation,
            std::shared_ptr<std::mutex> audio_context_lock);

  static bool IsValidChannelCount(const uint32_t &channel_count);

  static bool
  ConvertToChannelCountMode(const std::string &str_channel_countmode,
                            ChannelCountMode &channel_count_mode);

  static bool
  ConvertToChannelInterpretation(const std::string &str_channel_interpretation,
                                 ChannelInterpretation &channel_interpretation);

  static std::string
  ConvertChannelCountModeToString(const ChannelCountMode &channel_count_mode);

  static std::string ConvertChannelInterpretationToString(
      const ChannelInterpretation &channel_interpretation);

  uint32_t number_of_inputs() const;
  uint32_t number_of_outputs() const;
  uint32_t channel_count() const;
  ChannelCountMode channel_count_mode() const;
  ChannelInterpretation channel_interpretation() const;

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
  const ChannelCountMode channel_count_mode_;
  const ChannelInterpretation channel_interpretation_;
  std::shared_ptr<std::mutex> audio_context_lock_;

public:
  static const uint32_t kMinChannelCount;
  static const uint32_t kMaxChannelCount;
};
} // namespace js_audio