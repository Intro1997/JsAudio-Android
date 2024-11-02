#include "OscillatorNode.hpp"
#include "logger.hpp"
#include <cmath>

namespace js_audio {

const float OscillatorNode::kDetuneMax =
    1200 * log2f(std::numeric_limits<float>::max());
const float OscillatorNode::kDetuneMin =
    -1200 * log2f(std::numeric_limits<float>::max());
const uint32_t OscillatorNode::kNumberOfInputs = 0;
const uint32_t OscillatorNode::kNumberOfOutputs = 1;

const OscillatorNode::OscillatorType OscillatorNode::kDefaultType =
    OscillatorType::kSine;
const float OscillatorNode::kDefaultDetune = 0.0f;
const float OscillatorNode::kDefaultFrequency = 440.0f;
const uint32_t OscillatorNode::kDefaultChannelCount = 2;
const OscillatorNode::ChannelCountMode
    OscillatorNode::kDefaultChannelCountMode = ChannelCountMode::kMax;
const OscillatorNode::ChannelInterpretation
    OscillatorNode::kDefaultChannelInterpretation =
        ChannelInterpretation::kSpeakers;

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
  OscillatorNodeConstructHelper(
      std::shared_ptr<std::mutex> audio_context_lock_ref,
      const OscillatorOptions &options, const float &sample_rate)
      : OscillatorNode(audio_context_lock_ref, options, sample_rate) {}
};

OscillatorNode::OscillatorNode(
    std::shared_ptr<std::mutex> audio_context_lock_ref,
    const OscillatorOptions &options, const float &sample_rate)
    : AudioScheduledSourceNode(
          kNumberOfInputs, kNumberOfOutputs, options.channel_count,
          options.channel_count_mode, options.channel_interpretation,
          audio_context_lock_ref),
      type_(options.type), sample_rate_(sample_rate), current_time_(0) {}

std::shared_ptr<OscillatorNode> OscillatorNode::CreateOscillatorNode(
    const float &sample_rate,
    std::shared_ptr<std::mutex> audio_context_lock_ref) {
  OscillatorOptions options = GetDefaultOptions();
  return CreateOscillatorNode(options, sample_rate, audio_context_lock_ref);
}

std::shared_ptr<OscillatorNode> OscillatorNode::CreateOscillatorNode(
    const OscillatorOptions &options, const float &sample_rate,
    std::shared_ptr<std::mutex> audio_context_lock_ref) {
  std::shared_ptr<OscillatorNode> oscillator_node_ref =
      std::make_shared<OscillatorNodeConstructHelper>(audio_context_lock_ref,
                                                      options, sample_rate);

  std::weak_ptr<OscillatorNode> oscillator_node_weak_ref = oscillator_node_ref;

  std::function<AudioParam::SetterCallbackFunc> setter_cb = [=](const float &) {
    if (!oscillator_node_weak_ref.expired()) {
      oscillator_node_weak_ref.lock()->UpdateComputedFreq();
    }
  };

  const float min_frequency = -sample_rate / 2;
  const float max_frequency = sample_rate / 2;

  oscillator_node_ref->frequency_ref_ = std::make_shared<AudioParam>(
      AudioParam::K_RATE,
      std::clamp(options.frequency, min_frequency, max_frequency),
      min_frequency, max_frequency, audio_context_lock_ref, setter_cb);

  oscillator_node_ref->detune_ref_ = std::make_shared<AudioParam>(
      AudioParam::K_RATE, options.detune, kDetuneMin, kDetuneMax,
      audio_context_lock_ref, setter_cb);

  oscillator_node_ref->UpdateComputedFreq();

  return oscillator_node_ref;
}

void OscillatorNode::ProduceSamples(const size_t &sample_size,
                                    std::vector<std::vector<float>> &output) {
  if (state() != State::Start) {
    FillWithZeros(sample_size, output);
    return;
  }
  // Warn: you should not use any getter api here!
  // we have locked in AudioContext.ProduceSamples() outside!
  if (output.size() < 2) {
    output.resize(2);
  }
  for (auto &channel : output) {
    if (channel.size() < sample_size) {
      channel.resize(sample_size, 0);
    }
  }
  CreateWaveform(type_, sample_size, output[0]);
  output[1].assign(output[0].begin(), output[0].end());
}

OscillatorNode::OscillatorOptions OscillatorNode::GetDefaultOptions() {
  OscillatorOptions options;
  options.detune = kDefaultDetune;
  options.frequency = kDefaultFrequency;
  options.type = kDefaultType;
  options.channel_count = kDefaultChannelCount;
  options.channel_count_mode = kDefaultChannelCountMode;
  options.channel_interpretation = kDefaultChannelInterpretation;
  return options;
}

bool OscillatorNode::ConvertToOscillatorType(const std::string &str_type,
                                             OscillatorType &type) {
  if (str_type == "sine") {
    type = OscillatorType::kSine;
    return true;
  } else if (str_type == "sawtooth") {
    type = OscillatorType::kSawtooth;
    return true;
  } else if (str_type == "square") {
    type = OscillatorType::kSquare;
    return true;
  } else if (str_type == "triangle") {
    type = OscillatorType::kTriangle;
    return true;
  }
  return false;
}

float OscillatorNode::ClampToValidDetune(const float &detune) {
  return std::clamp(detune, kDetuneMin, kDetuneMax);
}

std::shared_ptr<AudioParam> OscillatorNode::frequency_ref() const {
  std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
  return frequency_ref_;
}

std::shared_ptr<AudioParam> OscillatorNode::detune_ref() const {
  std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
  return detune_ref_;
}

const char *OscillatorNode::type() const {
  return ConvertTypeToString(inner_type());
}

const WaveProducer::WaveType &OscillatorNode::inner_type() const {
  std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
  return type_;
}

void OscillatorNode::set_type(const std::string &type) {
  std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
  if (!ConvertToOscillatorType(type, type_)) {
    LOGE("Set oscillator node type error: Use unknown type %s\n", type.c_str());
  }
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
                                    std::vector<float> &output) {
  switch (type) {
  case WaveProducer::WaveType::kSine: {
    WaveProducer::CreateSineWave<float>(step_, current_time_, sign_,
                                        sample_size, output, 1.0);
    break;
  }
  case WaveProducer::WaveType::kSawtooth: {
    WaveProducer::CreateSawtoothWave<float>(step_, current_time_, sign_,
                                            sample_size, output, 1.0);
    break;
  }
  case WaveProducer::WaveType::kSquare: {
    WaveProducer::CreateSquareWave<float>(step_, current_time_, sign_,
                                          sample_size, output, 1.0);
    break;
  }
  case WaveProducer::WaveType::kTriangle: {
    WaveProducer::CreateTriangleWave<float>(step_, current_time_, sign_,
                                            sample_size, output, 1.0);
    break;
  }
  default: {
    break;
  }
  }
}

void OscillatorNode::UpdateComputedFreq() {
  const float freq_value = frequency_ref()->value();
  const float detune_value = detune_ref()->value();

  std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
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
