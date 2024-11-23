#include "OscillatorNode.hpp"
#include "BaseAudioContext.hpp"
#include "logger.hpp"
#include <cmath>

#define DOUBLE_M_PI (M_PI * 2)

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
          audio_context_lock_ref, sample_rate),
      type_(options.type), current_time_(0) {}

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
      "Oscillator.frequency",
      std::clamp(options.frequency, min_frequency, max_frequency),
      AudioParam::K_RATE, kDefaultFrequency, min_frequency, max_frequency,
      audio_context_lock_ref, setter_cb);

  oscillator_node_ref->detune_ref_ = std::make_shared<AudioParam>(
      "Oscillator.detune", options.detune, AudioParam::K_RATE, kDefaultDetune,
      kDetuneMin, kDetuneMax, audio_context_lock_ref, setter_cb);

  oscillator_node_ref->UpdateComputedFreq();

  return oscillator_node_ref;
}

void OscillatorNode::ProduceSamples(const size_t &sample_size,
                                    std::vector<std::vector<float>> &output) {
  if (start_time_ >= stop_time_ || state() != State::Start) {
    FillWithZeros(sample_size, output);
    return;
  }

  MakeOutputValid(sample_size, output);

  size_t front_zero_sample_size = 0;
  size_t process_sample_size = 0;
  size_t back_zero_sample_size = 0;
  GetOutputRegionSize(sample_size, front_zero_sample_size, process_sample_size,
                      back_zero_sample_size);
  AssignOutputWithZeros(output, 0, front_zero_sample_size);
  Process(output, front_zero_sample_size, process_sample_size);
  AssignOutputWithZeros(output, front_zero_sample_size + process_sample_size,
                        back_zero_sample_size);
  if (back_zero_sample_size >= sample_size) {
    set_state(State::Stop);
  }
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

void OscillatorNode::MakeOutputValid(const size_t &sample_size,
                                     std::vector<std::vector<float>> &output) {
  if (output.size() < channel_count_) {
    output.resize(channel_count_);
  }
  for (auto &channel : output) {
    if (channel.size() < sample_size) {
      channel.resize(sample_size, 0);
    }
  }
}

void OscillatorNode::GetOutputRegionSize(const size_t sample_size,
                                         size_t &front_zero_sample_size,
                                         size_t &process_sample_size,
                                         size_t &back_zero_sample_size) {
  front_zero_sample_size = 0;
  process_sample_size = 0;
  back_zero_sample_size = 0;

  if (!sample_size) {
    return;
  }

  auto base_audio_context_ref = base_audio_context_ptr_.lock();
  if (!base_audio_context_ref) {
    LOGE("Invalid audio context reference!\n");
    return;
  }

  // clang-format off
  /** 
   * Graph
   *           front_zero_sample_size    process_sample_size      back_zero_sample_size
   *                 ⬇️                     ⬇️                     ⬇️
   *          |__________________｜_________________________|________________|
   *          ⬆️                  ⬆️                        ⬆️                ⬆️
   *   start_process_time        start_time_           stop_time         end_process_time
   */
  // clang-format on

  const double start_produce_time = base_audio_context_ref->current_time();
  const double output_time = (double)sample_size / seconds_per_sample_;
  const double end_produce_time = start_produce_time + output_time;

  // calculate front zeros count
  if (end_produce_time <= start_time_) {
    front_zero_sample_size = sample_size;
    return;
  } else if (start_produce_time < start_time_) {
    front_zero_sample_size =
        (start_time_ - start_produce_time) * (double)seconds_per_sample_;
  }

  // calculate back zeros count
  if (end_produce_time > stop_time_) {
    back_zero_sample_size =
        (end_produce_time - stop_time_) * (double)seconds_per_sample_;
  }

  // calculate process smaple count
  process_sample_size =
      sample_size - front_zero_sample_size - back_zero_sample_size;
}

bool OscillatorNode::AssignOutputWithZeros(
    std::vector<std::vector<float>> &output, const size_t &begin,
    const size_t &sample_size) {
  if (!sample_size) {
    return true;
  }
  if (output.empty()) {
    LOGE("Empty output!\n");
    return false;
  }
  const size_t assign_cnt = begin + sample_size;
  for (auto &channel : output) {
    if (channel.size() <= begin || channel.size() < assign_cnt) {
      LOGE("Try to assign channel<size: %zu> with begin<%zu> or "
           "sample_size<%zu>!\n",
           channel.size(), begin, begin + sample_size);
      return false;
    }
  }

  for (auto &channel : output) {
    std::fill(channel.begin() + begin, channel.begin() + assign_cnt, 0);
  }
  return true;
}

void OscillatorNode::Process(std::vector<std::vector<float>> &output,
                             const size_t &begin, const size_t &sample_size) {
  if (output.empty()) {
    LOGE("Output buffer has channel zero!\n");
    return;
  }

  std::vector<float> channel_output;
  CreateWaveform(type_, sample_size, channel_output);
  for (auto &channel : output) {
    if (sample_size + begin > channel.size()) {
      LOGE("Write data length<begin: %zu, sample_size:%zu> largest than output "
           "buffer size<%zu>!\n",
           begin, sample_size, channel.size());
      return;
    }
    std::copy(channel_output.begin(), channel_output.end(),
              channel.begin() + begin);
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
  if (!frequency_ref() || !detune_ref()) {
    return;
  }
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
