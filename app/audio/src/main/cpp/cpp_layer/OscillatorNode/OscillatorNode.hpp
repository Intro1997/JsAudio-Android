#pragma once
#include "AudioParam.hpp"
#include "AudioScheduledSourceNode.hpp"
#include "WaveProducer.hpp"

namespace js_audio {
class OscillatorNode : public AudioScheduledSourceNode {
public:
  static std::shared_ptr<OscillatorNode>
  CreateOscillatorNode(std::shared_ptr<std::mutex> audio_context_lock,
                       const float &sample_rate);

  void ProduceSamples(size_t sample_size,
                      std::vector<SLint16> &output) override;

  void set_type(const std::string &type);

  std::shared_ptr<AudioParam> frequency() const;
  std::shared_ptr<AudioParam> detune() const;
  const char *type() const;

protected:
  OscillatorNode(std::shared_ptr<std::mutex> audio_context_lock,
                 const float &sample_rate);

private:
  const WaveProducer::WaveType &inner_type() const;

  WaveProducer::WaveType ConvertStringToType(const std::string &type) const;
  const char *ConvertTypeToString(const WaveProducer::WaveType &type) const;
  void UpdateComputedFreq();

  void CreateWaveform(const WaveProducer::WaveType &type,
                      const size_t &sample_size, std::vector<SLint16> &output);

  std::shared_ptr<AudioParam> frequency_;
  std::shared_ptr<AudioParam> detune_;
  WaveProducer::WaveType type_;

  const float sample_rate_;
  float computed_freq_;
  float sign_;
  float step_;
  double current_time_; // should belong to [0, 2*pi]

  static const float kDetuneMax;
  static const float kDetuneMin;
  static const uint32_t kNumberOfInputs;
  static const uint32_t kNumberOfOutputs;
  static const uint32_t kChannelCount;
  static const std::string kChannelCountMode;
  static const std::string kChannelInterpretation;
};
} // namespace  js_audio
