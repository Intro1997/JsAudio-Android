#pragma once
#include "AudioParam.hpp"
#include "AudioScheduledSourceNode.hpp"
#include "WaveProducer.hpp"

namespace js_audio {
class OscillatorNode : public AudioScheduledSourceNode {
public:
  using OscillatorType = WaveProducer::WaveType;
  using ChannelCountMode = AudioNode::ChannelCountMode;
  using ChannelInterpretatio = AudioNode::ChannelInterpretation;

  struct OscillatorOptions : public AudioNode::AudioNodeOptions {
    float detune;
    float frequency;
    OscillatorType type;
  };

  static std::shared_ptr<OscillatorNode>
  CreateOscillatorNode(const float &sample_rate,
                       std::shared_ptr<std::mutex> audio_context_lock_ref);
  static std::shared_ptr<OscillatorNode>
  CreateOscillatorNode(const OscillatorOptions &options,
                       const float &sample_rate,
                       std::shared_ptr<std::mutex> audio_context_lock_ref);

  void ProduceSamples(const size_t &sample_size,
                      std::vector<std::vector<float>> &output) override;

  static OscillatorOptions GetDefaultOptions();

  static bool ConvertToOscillatorType(const std::string &str_type,
                                      OscillatorType &type);

  static float ClampToValidDetune(const float &detune);
  static float ClampToValidFrequency(const float &frequency);

  void set_type(const std::string &type);

  std::shared_ptr<AudioParam> frequency_ref() const;
  std::shared_ptr<AudioParam> detune_ref() const;
  const char *type() const;

protected:
  OscillatorNode(std::shared_ptr<std::mutex> audio_context_lock_ref,
                 const OscillatorOptions &options, const float &sample_rate);

private:
  const WaveProducer::WaveType &inner_type() const;
  
  void MakeOutputValid(const size_t &sample_size,
                       std::vector<std::vector<float>> &output);
  void GetOutputRegionSize(const size_t sample_size,
                           size_t &front_zero_sample_size,
                           size_t &process_sample_size,
                           size_t &back_zero_sample_size);
  bool AssignOutputWithZeros(std::vector<std::vector<float>> &output,
                             const size_t &begin, const size_t &sample_size);
  void Process(std::vector<std::vector<float>> &output, const size_t &begin,
               const size_t &sample_size);

  const char *ConvertTypeToString(const WaveProducer::WaveType &type) const;
  void UpdateComputedFreq();

  void CreateWaveform(const WaveProducer::WaveType &type,
                      const size_t &sample_size, std::vector<float> &output);

  std::shared_ptr<AudioParam> frequency_ref_;
  std::shared_ptr<AudioParam> detune_ref_;
  WaveProducer::WaveType type_;

  float computed_freq_;
  float sign_;
  float step_;
  double current_time_; // should belong to [0, 2*pi]

  static const float kDetuneMax;
  static const float kDetuneMin;
  static const uint32_t kNumberOfInputs;
  static const uint32_t kNumberOfOutputs;

  static const OscillatorType kDefaultType;
  static const float kDefaultDetune;
  static const float kDefaultFrequency;
  static const uint32_t kDefaultChannelCount;
  static const ChannelCountMode kDefaultChannelCountMode;
  static const ChannelInterpretation kDefaultChannelInterpretation;
};
} // namespace  js_audio
