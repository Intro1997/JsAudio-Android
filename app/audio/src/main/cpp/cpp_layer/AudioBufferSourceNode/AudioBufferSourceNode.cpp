#include "AudioBufferSourceNode.hpp"
#include "BaseAudioContext.hpp"
#include "logger.hpp"
#include "timer.hpp"
#include <cmath>

namespace js_audio {
using AudioBufferSourceOptions =
    AudioBufferSourceNode::AudioBufferSourceOptions;
using ChannelCountMode = AudioNode::ChannelCountMode;
using ChannelInterpretation = AudioNode::ChannelInterpretation;

class AudioBufferSourceNodeConstructHelper : public AudioBufferSourceNode {
public:
  AudioBufferSourceNodeConstructHelper(
      const AudioBufferSourceOptions &options, const float &sample_rate,
      std::shared_ptr<std::mutex> audio_context_lock_ref)
      : AudioBufferSourceNode(options, sample_rate, audio_context_lock_ref) {}
};

float AudioBufferSourceNode::kDefaultDetune = 0;
bool AudioBufferSourceNode::kDefaultLoop = false;
double AudioBufferSourceNode::kDefaultLoopEnd = 0;
double AudioBufferSourceNode::kDefaultLoopStart = 0;
float AudioBufferSourceNode::kDefaultPlaybackRate = 1;
uint32_t AudioBufferSourceNode::kDefaultNumberOfInputs = 0;
uint32_t AudioBufferSourceNode::kDefaultNumberOfOutputs = 1;
uint32_t AudioBufferSourceNode::kDefaultChannelCount = 2;
ChannelCountMode AudioBufferSourceNode::kDefaultChannelCountMode =
    ChannelCountMode::kMax;
ChannelInterpretation AudioBufferSourceNode::kDefaultChannelInterpretation =
    ChannelInterpretation::kSpeakers;

std::shared_ptr<AudioBufferSourceNode>
AudioBufferSourceNode::CreateAudioBufferSourceNode(
    const float &sample_rate,
    std::shared_ptr<std::mutex> audio_context_lock_ref) {
  return CreateAudioBufferSourceNode(GetDefaultOptions(), sample_rate,
                                     audio_context_lock_ref);
}

std::shared_ptr<AudioBufferSourceNode>
AudioBufferSourceNode::CreateAudioBufferSourceNode(
    const AudioBufferSourceOptions &options, const float &sample_rate,
    std::shared_ptr<std::mutex> audio_context_lock_ref) {
  auto audio_buffer_source_node_ref =
      std::make_shared<AudioBufferSourceNodeConstructHelper>(
          options, sample_rate, audio_context_lock_ref);
  std::weak_ptr<AudioBufferSourceNode> audio_buffer_source_node_ptr =
      audio_buffer_source_node_ref;
  std::function<AudioParam::SetterCallbackFunc> setter_cb = [=](const float &) {
    if (auto ref = audio_buffer_source_node_ptr.lock()) {
      ref->UpdateComputePlaybackRate();
    }
  };

  std::shared_ptr<AudioParam> detune_ref = std::make_shared<AudioParam>(
      "AudioBufferSourceNode.detune", AudioParam::K_RATE, options.detune,
      -FLT_MAX, FLT_MAX, audio_context_lock_ref, setter_cb);
  std::shared_ptr<AudioParam> playback_rate_ref = std::make_shared<AudioParam>(
      "AudioBufferSourceNode.playbackRate", AudioParam::K_RATE,
      options.playback_rate, -FLT_MAX, FLT_MAX, audio_context_lock_ref,
      setter_cb);

  audio_buffer_source_node_ref->set_detune_ref(detune_ref);
  audio_buffer_source_node_ref->set_playback_rate_ref(playback_rate_ref);

  audio_buffer_source_node_ref->UpdateComputePlaybackRate();

  return audio_buffer_source_node_ref;
}

AudioBufferSourceOptions AudioBufferSourceNode::GetDefaultOptions() {
  AudioBufferSourceOptions options;
  options.buffer_ref = nullptr;
  options.detune = kDefaultDetune;
  options.loop = kDefaultLoop;
  options.loop_end = kDefaultLoopEnd;
  options.loop_start = kDefaultLoopStart;
  options.playback_rate = kDefaultPlaybackRate;
  return options;
}

void AudioBufferSourceNode::Start(const double &when, const double &offset,
                                  const double &duration) {
  if (std::isnan(when) || std::isnan(offset) || std::isnan(duration) ||
      when < 0 || offset < 0) {
    LOGE("Invalid parameters: when:%f, offset:%f, duration:%f\n", when, offset,
         duration);
    return;
  }

  if (state() == State::Stop && has_started_) {
    return;
  }

  AudioScheduledSourceNode::Start(when);

  {
    std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
    playing_time_ = current_time_ = offset_ = offset;
    duration_ = duration < 0 ? INFINITY : duration;
    stop_time_ = offset_ + duration_;
  }
}
bool AudioBufferSourceNode::AssignOutputWithZeros(
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

void AudioBufferSourceNode::GetOutputRegionSize(const size_t sample_size,
                                                size_t &front_zero_sample_size,
                                                size_t &process_sample_size,
                                                size_t &back_zero_sample_size) {
  front_zero_sample_size = 0;
  process_sample_size = 0;
  back_zero_sample_size = 0;

  auto base_audio_context_ref = base_audio_context_ptr_.lock();
  if (!base_audio_context_ref) {
    return;
  }

  // clang-format off
  /** 
   * Graph
   *             front_zero_duration       process_duration      end_process_time
   *                 ⬇️                     ⬇️                     ⬇️
   *          |__________________｜_________________________|________________|
   *          ⬆️                  ⬆️                        ⬆️                ⬆️
   *   start_process_time        start_time_           stop_time         end_process_time
   */
  // clang-format on

  const double start_process_time = base_audio_context_ref->current_time();
  const double produce_duration = sample_size * seconds_per_sample_;
  const double end_process_time = start_process_time + produce_duration;

  // 1. calculate front_zero_sample_size
  const double front_zero_duration = start_time_ - start_process_time;
  if (front_zero_duration > 0) {
    front_zero_sample_size = front_zero_duration / seconds_per_sample_;
    if (front_zero_sample_size >= sample_size) {
      front_zero_sample_size = sample_size;
      return;
    }
  }

  // 2. calculate back_zero_sample_size
  const double stop_time = start_time_ + offset_ + duration_;
  const double back_zeros_duration = end_process_time - stop_time;
  if (back_zeros_duration > 0) {
    back_zero_sample_size = back_zeros_duration / seconds_per_sample_;
    if (back_zero_sample_size >= sample_size) {
      back_zero_sample_size = sample_size;
      return;
    }
  }

  // 3. check if need process sample
  if (front_zero_sample_size + back_zero_sample_size >= sample_size) {
    return;
  }

  // finally calculate process_sample_size
  process_sample_size =
      sample_size - (front_zero_sample_size + back_zero_sample_size);
}

void AudioBufferSourceNode::ProduceSamples(
    const size_t &sample_size, std::vector<std::vector<float>> &output) {
  double actual_loop_start;
  double actual_loop_end;
  if (GetActualLoopEndingPoint(actual_loop_start, actual_loop_end) &&
      CanPlay(actual_loop_start, actual_loop_end)) {
    MakeOutputValid(sample_size, output);
    size_t front_zero_sample_size = 0;
    size_t process_sample_size = 0;
    size_t back_zero_sample_size = 0;
    GetOutputRegionSize(sample_size, front_zero_sample_size,
                        process_sample_size, back_zero_sample_size);

    AssignOutputWithZeros(output, 0, front_zero_sample_size);
    Process(output, front_zero_sample_size, process_sample_size,
            actual_loop_start, actual_loop_end);
    AssignOutputWithZeros(output, front_zero_sample_size + process_sample_size,
                          back_zero_sample_size);
    if (back_zero_sample_size == sample_size) {
      set_state(State::Stop);
    }
  } else {
    AudioNode::FillWithZeros(audio_buffer_ref_
                                 ? audio_buffer_ref_->number_of_channel()
                                 : channel_count_,
                             sample_size, output);
  }
}

void AudioBufferSourceNode::Process(std::vector<std::vector<float>> &output,
                                    const size_t &begin,
                                    const size_t &sample_size,
                                    const double &actual_loop_start,
                                    const double &actual_loop_end) {
  if (!sample_size) {
    return;
  }
  if (output.empty()) {
    LOGE("Empty output!\n");
    return;
  }
  const size_t assign_cnt = begin + sample_size;
  for (auto &channel : output) {
    if (channel.size() <= begin || channel.size() < assign_cnt) {
      LOGE("Try to assign channel<size: %zu> with begin<%zu> or "
           "sample_size<%zu>!\n",
           channel.size(), begin, begin + sample_size);
      return;
    }
  }

  // TODO: do optimize here!
  for (size_t i = begin; i < assign_cnt; i++) {
    for (size_t c = 0; c < output.size(); c++) {
      float sample_data =
          GetCurrentTimeOutputSample(actual_loop_start, actual_loop_end, c);
      output[c][i] = sample_data;
    }
    // TODO: move to update time
    current_time_ += seconds_per_sample_;
    playing_time_ += compute_playback_rate() * seconds_per_sample_;
  }
}

const std::shared_ptr<const AudioBuffer>
AudioBufferSourceNode::audio_buffer_ref() const {
  std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
  return audio_buffer_ref_;
}

void AudioBufferSourceNode::set_audio_buffer_ref(
    std::shared_ptr<AudioBuffer> audio_buffer_ref) {
  std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
  audio_buffer_ref_ = audio_buffer_ref;
}

std::shared_ptr<AudioParam> AudioBufferSourceNode::detune_ref() const {
  return detune_ref_;
}

void AudioBufferSourceNode::set_detune_ref(
    const std::shared_ptr<AudioParam> &detune_ref) {
  detune_ref_ = detune_ref;
}

bool AudioBufferSourceNode::loop() const {
  std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
  return loop_;
}

void AudioBufferSourceNode::set_loop(const bool &loop) {
  std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
  loop_ = loop;
}

double AudioBufferSourceNode::loop_end() const {
  std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
  return loop_end_;
}

void AudioBufferSourceNode::set_loop_end(const double &loop_end) {
  std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
  loop_end_ = loop_end;
}

double AudioBufferSourceNode::loop_start() const {
  std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
  return loop_start_;
}

void AudioBufferSourceNode::set_loop_start(const double &loop_start) {
  std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
  loop_start_ = loop_start;
}

std::shared_ptr<AudioParam> AudioBufferSourceNode::playback_rate_ref() const {
  return playback_rate_ref_;
}

void AudioBufferSourceNode::set_playback_rate_ref(
    const std::shared_ptr<AudioParam> &playback_rate_ref) {
  playback_rate_ref_ = playback_rate_ref;
}

float AudioBufferSourceNode::compute_playback_rate() const {
  std::lock_guard<std::mutex> guard(compute_playback_rate_lock_);
  return compute_playback_rate_;
}

void AudioBufferSourceNode::UpdateComputePlaybackRate() {
  if (!detune_ref() || !playback_rate_ref()) {
    return;
  }
  float detune_value = detune_ref()->value();
  float playback_rate_value = playback_rate_ref()->value();
  {
    std::lock_guard<std::mutex> guard(compute_playback_rate_lock_);
    compute_playback_rate_ =
        playback_rate_value * std::pow(2, detune_value / 1200.0f);
  }
}

AudioBufferSourceNode::AudioBufferSourceNode(
    const AudioBufferSourceOptions &options, const float &sample_rate,
    std::shared_ptr<std::mutex> audio_context_lock_ref)
    : AudioScheduledSourceNode(kDefaultNumberOfInputs, kDefaultNumberOfOutputs,
                               kDefaultChannelCount, kDefaultChannelCountMode,
                               kDefaultChannelInterpretation,
                               audio_context_lock_ref, sample_rate),
      audio_buffer_ref_(options.buffer_ref), loop_(options.loop),
      loop_end_(options.loop_end), loop_start_(options.loop_start),
      current_time_(0), playing_time_(0), offset_(0), duration_(-1) {}

bool AudioBufferSourceNode::GetActualLoopEndingPoint(double &actual_loop_start,
                                                     double &actual_loop_end) {
  if (!audio_buffer_ref_) {
    return false;
  }
  const double buffer_duration = audio_buffer_ref_->duration();
  if (loop_start_ >= 0 && loop_end_ > 0 && loop_end_ <= buffer_duration &&
      loop_end_ > loop_start_) {
    actual_loop_start = loop_start_;
    actual_loop_end = loop_end_;
  } else {
    actual_loop_start = 0;
    actual_loop_end = buffer_duration;
  }
  return true;
}
bool AudioBufferSourceNode::CanPlay(const double &actual_loop_start,
                                    const double &actual_loop_end) {
  // check node is stop
  if (state() != State::Start || duration_ == 0 || !audio_buffer_ref_ ||
      base_audio_context_ptr_.expired()) {
    return false;
  }

  if (offset_ < 0 || current_time_ < 0) {
    LOGE("Invalid parameter!\n");
    return false;
  }
  if (loop_) {
    if (current_time_ >= stop_time_ || stop_time_ <= actual_loop_start) {
      return false;
    }
  } else {
    if (current_time_ >= std::min(stop_time_, audio_buffer_ref_->duration())) {
      return false;
    }
  }
  return true;
}

float AudioBufferSourceNode::GetCurrentTimeOutputSample(
    const double &actual_loop_start, const double &actual_loop_end,
    const size_t &channel) {
  float sample_data = 0;
  if (IsSameTime(current_time_, stop_time_) || current_time_ > stop_time_) {
    return sample_data;
  }

  if (loop_) {
    if (IsSameTime(playing_time_, actual_loop_end) ||
        playing_time_ > actual_loop_end) {
      playing_time_ = playing_time_ - actual_loop_end + actual_loop_start;
    }
    sample_data = GetCurrentTimeSampleFromBuffer(channel, actual_loop_start);
  } else {
    if (IsSameTime(playing_time_, audio_buffer_ref_->duration()) ||
        playing_time_ > audio_buffer_ref_->duration()) {
      return sample_data;
    }
    sample_data = GetCurrentTimeSampleFromBuffer(channel, 0);
  }

  return sample_data;
}

float AudioBufferSourceNode::GetCurrentTimeSampleFromBuffer(
    const size_t &channel, const double &actual_loop_start) {
  const double float_buffer_idx = playing_time_ / seconds_per_sample_;
  const size_t left_idx = std::floor(float_buffer_idx);
  size_t right_idx = std::ceil(float_buffer_idx);
  if (IsSameTime(float_buffer_idx, (double)left_idx)) {
    right_idx = left_idx;
  }

  if (right_idx >= audio_buffer_ref_->length()) {
    right_idx = std::floor(actual_loop_start / seconds_per_sample_);
  }

  if (std::max(left_idx, right_idx) >= audio_buffer_ref_->length()) {
    return 0.0f;
  }

  const auto *channel_data_ptr = audio_buffer_ref_->GetChannelDataPtr(channel);
  if (!channel_data_ptr) {
    LOGE("Invalid channel data ptr!\n");
    return 0.0;
  }

  if (left_idx == right_idx) {
    return channel_data_ptr->at(left_idx);
  }

  // TODO: optimize linear interpolation
  const double left_sample = channel_data_ptr->at(left_idx);
  const double right_sample = channel_data_ptr->at(right_idx);
  const double left_time = left_idx * seconds_per_sample_;
  const double interpolation_x = playing_time_ - left_time;
  const double interpolation_k =
      (right_sample - left_sample) / seconds_per_sample_;
  const double interpolation_y = interpolation_x * interpolation_k;
  return left_sample + interpolation_y;
}

void AudioBufferSourceNode::MakeOutputValid(
    const size_t &sample_size, std::vector<std::vector<float>> &output) {
  const uint32_t number_of_channel = audio_buffer_ref_->number_of_channel();
  if (output.size() < number_of_channel) {
    output.resize(number_of_channel);
  }
  for (auto &channel : output) {
    if (channel.size() < sample_size) {
      channel.resize(sample_size, 0);
    }
  }
}

} // namespace js_audio
