#include "OscillatorNode.hpp"
#include "logger.hpp"
#include <cmath>

namespace js_audio {

const float OscillatorNode::kDetuneMax = 153600.0f;
const float OscillatorNode::kDetuneMin = -153600.0f;
const uint32_t OscillatorNode::kNumberOfInputs = 0;
const uint32_t OscillatorNode::kNumberOfOutputs = 1;
const uint32_t OscillatorNode::kChannelCount = 2;
const std::string OscillatorNode::kChannelCountMode = "max";
const std::string OscillatorNode::kChannelInterpretation = "speakers";

#define PCM_SAMPLE_LENGTH 9
#define PCM_SAMPLE_CHANNEL 2
#define AUDIO_DATA_STORAGE_SIZE (44100 * PCM_SAMPLE_CHANNEL * PCM_SAMPLE_LENGTH)
#define AUDIO_DATA_BUFFER_SIZE (44100)
#define DOUBLE_M_PI (M_PI * 2)
#define DO_TONE_FREQ (261.6)
#define REI_TONE_FREQ (293.6)
#define MI_TONE_FREQ (329.6)
#define FA_TONE_FREQ (349.2)
#define SOL_TONE_FREQ (392)
#define DETUNE (0)
#define AUDIO_SEGMENT_DATA_SIZE 4096
#define TRIBLE_PI_2 3 * M_PI_2

static void CreateTestPcmData(size_t sample_size,
                              std::vector<SLint16> &output) {
  output.resize(sample_size * 2, 0);
  const double do_sine_step = DOUBLE_M_PI *
                              (DO_TONE_FREQ * std::pow(2.0, DETUNE / 1200.0)) /
                              (double)44100;
  for (size_t i = 0; i < sample_size; i++) {
    double ret = sin(do_sine_step * (double)i);
    output[i * 2] = output[i * 2 + 1] = ret * INT16_MAX;
  }
}

class OscillatorNodeConstructHelper : public OscillatorNode {
public:
  OscillatorNodeConstructHelper(std::shared_ptr<std::mutex> audio_context_lock,
                                const float &sample_rate)
      : OscillatorNode(audio_context_lock, sample_rate) {}
};

std::shared_ptr<OscillatorNode> OscillatorNode::CreateOscillatorNode(
    std::shared_ptr<std::mutex> audio_context_lock, const float &sample_rate) {
  std::shared_ptr<OscillatorNode> oscillator_node_ref =
      std::make_shared<OscillatorNodeConstructHelper>(audio_context_lock,
                                                      sample_rate);

  std::weak_ptr<OscillatorNode> oscillator_node_weak_ref = oscillator_node_ref;

  std::function<AudioParam::SetterCallbackFunc> setter_cb = [=](const float &) {
    if (!oscillator_node_weak_ref.expired()) {
      oscillator_node_weak_ref.lock()->UpdateComputedFreq();
    }
  };

  oscillator_node_ref->frequency_ = std::make_shared<AudioParam>(
      AudioParam::K_RATE, 440, -sample_rate / 2, sample_rate / 2,
      audio_context_lock, setter_cb);

  oscillator_node_ref->detune_ =
      std::make_shared<AudioParam>(AudioParam::K_RATE, 0, kDetuneMin,
                                   kDetuneMax, audio_context_lock, setter_cb);

  oscillator_node_ref->UpdateComputedFreq();

  return oscillator_node_ref;
}

OscillatorNode::OscillatorNode(std::shared_ptr<std::mutex> audio_context_lock,
                               const float &sample_rate)
    : AudioScheduledSourceNode(kNumberOfInputs, kNumberOfOutputs, kChannelCount,
                               kChannelCountMode, kChannelInterpretation,
                               audio_context_lock),
      type_(WaveProducer::WaveType::kSine), sample_rate_(sample_rate),
      current_time_(0) {}

void OscillatorNode::ProduceSamples(size_t sample_size,
                                    std::vector<SLint16> &output) {
  // Warn: you should not use any getter api here!
  // we have locked in AudioContext.ProduceSamples() outside!
  CreateWaveform(type_, sample_size, output);
}

std::shared_ptr<AudioParam> OscillatorNode::frequency() const {
  std::lock_guard<std::mutex> guard(*audio_context_lock_);
  return frequency_;
}

std::shared_ptr<AudioParam> OscillatorNode::detune() const {
  std::lock_guard<std::mutex> guard(*audio_context_lock_);
  return detune_;
}

const char *OscillatorNode::type() const {
  return ConvertTypeToString(inner_type());
}

const WaveProducer::WaveType &OscillatorNode::inner_type() const {
  std::lock_guard<std::mutex> guard(*audio_context_lock_);
  return type_;
}

void OscillatorNode::set_type(const std::string &type) {
  const WaveProducer::WaveType oscillator_node_type = ConvertStringToType(type);
  if (oscillator_node_type != WaveProducer::WaveType::kUnknown) {
    std::lock_guard<std::mutex> guard(*audio_context_lock_);
    type_ = oscillator_node_type;
  } else {
    LOGE("Error! Unsupported oscillator type: %s\n", type.c_str());
  }
}

WaveProducer::WaveType
OscillatorNode::ConvertStringToType(const std::string &type) const {
  if (type == "sawtooth") {
    return WaveProducer::WaveType::kSawtooth;
  }
  if (type == "sine") {
    return WaveProducer::WaveType::kSine;
  }
  if (type == "square") {
    return WaveProducer::WaveType::kSquare;
  }
  if (type == "triangle") {
    return WaveProducer::WaveType::kTriangle;
  }
  return WaveProducer::WaveType::kUnknown;
}

const char *
OscillatorNode::ConvertTypeToString(const WaveProducer::WaveType &type) const {
  switch (type) {
  case WaveProducer::WaveType::kSine: {
    return "sine";
  }
  case WaveProducer::WaveType::kSawtooth: {
    return "sawtooth";
  }
  case WaveProducer::WaveType::kSquare: {
    return "square";
  }
  case WaveProducer::WaveType::kTriangle: {
    return "triangle";
  }
  default: {
    return "unknown";
  }
  }
}

void OscillatorNode::CreateWaveform(const WaveProducer::WaveType &type,
                                    const size_t &sample_size,
                                    std::vector<SLint16> &output) {
  switch (type) {
  case WaveProducer::WaveType::kSine: {
    WaveProducer::CreateSineWave<SLint16>(step_, current_time_, sign_,
                                          sample_size, output, INT16_MAX);
    break;
  }
  case WaveProducer::WaveType::kSawtooth: {
    WaveProducer::CreateSawtoothWave<SLint16>(step_, current_time_, sign_,
                                              sample_size, output, INT16_MAX);
    break;
  }
  case WaveProducer::WaveType::kSquare: {
    WaveProducer::CreateSquareWave<SLint16>(step_, current_time_, sign_,
                                            sample_size, output, INT16_MAX);
    break;
  }
  case WaveProducer::WaveType::kTriangle: {
    WaveProducer::CreateTriangleWave<SLint16>(step_, current_time_, sign_,
                                              sample_size, output, INT16_MAX);
    break;
  }
  default: {
    break;
  }
  }
}

void OscillatorNode::UpdateComputedFreq() {
  const float freq_value = frequency()->value();
  const float detune_value = detune()->value();

  std::lock_guard<std::mutex> guard(*audio_context_lock_);
  computed_freq_ = freq_value * std::exp2(detune_value / 1200);

  const float abs_freq = std::abs(computed_freq_);

  sign_ = computed_freq_ > 0.0 ? 1.0 : -1.0;
  if (abs_freq >= sample_rate_ / 2.0f) {
    step_ = 0.0f;
  } else {
    step_ = abs_freq * DOUBLE_M_PI / sample_rate_;
  }
}

} // namespace js_audio
