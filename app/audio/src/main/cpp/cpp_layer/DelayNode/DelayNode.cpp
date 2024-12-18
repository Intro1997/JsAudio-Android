#include "DelayNode.hpp"
#include "BaseAudioContext.hpp"
#include "logger.hpp"
#include <cmath>

namespace js_audio {

uint32_t DelayNode::kNumberOfInputs = 1;
uint32_t DelayNode::kNumberOfOutputs = 1;
uint32_t DelayNode::kMaxOfMaxDelayTime = 180;
uint32_t DelayNode::kMinOfMaxDelayTime = 0;

class DelayNodeConstructHelper : public DelayNode {
public:
  DelayNodeConstructHelper(const DelayOptions &options,
                           const float &sample_rate,
                           std::shared_ptr<std::mutex> audio_context_lock_ref)
      : DelayNode(options, sample_rate, audio_context_lock_ref) {}
};

std::shared_ptr<DelayNode>
DelayNode::CreateDelayNode(const DelayOptions &options,
                           const float &sample_rate,
                           std::shared_ptr<std::mutex> audio_context_lock_ref) {
  auto delay_node_ref = std::make_shared<DelayNodeConstructHelper>(
      options, sample_rate, audio_context_lock_ref);
  std::weak_ptr<DelayNode> delay_node_ptr = delay_node_ref;

  std::function<AudioParam::SetterCallbackFunc> setter_cb =
      [=](const float &value) {
        if (auto ref = delay_node_ptr.lock()) {
          ref->set_delay_time_value(value);
        }
      };

  delay_node_ref->delay_time_ref_ = std::make_shared<AudioParam>(
      "Delay.delayTime", options.delay_time, AudioParam::A_RATE, 0, 0,
      options.max_delay_time, audio_context_lock_ref, setter_cb);

  delay_node_ref->set_delay_time_value(
      delay_node_ref->delay_time_ref_->value());

  return delay_node_ref;
}

DelayNode::DelayOptions DelayNode::GetDefaultOptions() {
  DelayOptions options;
  options.max_delay_time = 1;
  options.delay_time = 0;
  options.channel_count = 2;
  options.channel_count_mode = AudioNode::ChannelCountMode::kMax;
  options.channel_interpretation = AudioNode::ChannelInterpretation::kSpeakers;
  return options;
}

std::shared_ptr<AudioParam> DelayNode::delay_time_ref() const {
  return delay_time_ref_;
}

void DelayNode::set_delay_time_value(const float &delay_time_value) {

  std::lock_guard<std::mutex> guard(delay_time_lock_);
  delay_time_value_ = delay_time_value;
}

void DelayNode::Process(const size_t &sample_size,
                        std::vector<std::vector<float>> &output) {
  // clang-format off
  /**
   * Algorithm Description
   * 1. Append new output to buffer accroding to current_sample_index
   * 2. Update delay_sample_index (which should vary accroding to delay time set
   * by user in js code)
   * 3. Read inner buffer according to delay_sample_index and sample_size
   *
   *
   * DelayBuffer Produce Process
   * let c_idx = current_sample_index
   * let d_idx = delay_sample_index
   * let s_size = sample_size
   *
   *      c_idx   c_idx+s_size       d_idx     c_idx+s_size
   *        v           v               v           v
   *        |===========|_______________|===========|_______________|  <-- delay_buffer
   *              ^                           ^    
   *           s_size                       s_size    
   *         
   * 0. delay_buffer.resize(inner_buffer.size() * 2)
   * loop start
   *    1. c_idx = 0
   *    2. d_idx = delay_buffer.size() / 2
   *    3. write s_size new data to delay_buffer begin with c_idx
   *    4. update c_idx to (c_idx + s_size) % delay_buffer.size()
   *    5. read s_size inner data to output_buffer begin with d_idx
   *    6. update d_idx to (d_idx + s_size) % delay_buffer.size()
   * loop end
   */
  // clang-format on

  if (src_audio_node_ref_) {
    src_audio_node_ref_->ProduceSamples(sample_size, output);
    MakeBufferInvalid(output.size());
    UpdateDelaySampleIdx(delay_time_value());
    WriteOutputToBuffer(sample_size, output);
    ReadBufferToOutput(sample_size, output);
  } else {
    AudioNode::FillWithZeros(sample_size, output);
  }
}

float DelayNode::delay_time_value() const {
  std::lock_guard<std::mutex> guard(delay_time_lock_);
  return delay_time_value_;
}

void DelayNode::UpdateDelaySampleIdx(const float &delay_time) {
  const uint32_t delay_samples = (uint32_t)sample_rate_ * delay_time;
  if (buffer_[0].size() < delay_samples + (uint32_t)sample_rate_) {
    LOGE("Inner channel buffer size<%zu> is less than (delay_samples<%u> + "
         "sample_rate<%f>)<%u>! DelayNode cannot handle this situation\n",
         buffer_[0].size(), delay_samples, sample_rate_,
         delay_samples + (uint32_t)sample_rate_);
    return;
  }
  if (current_sample_idx_ < delay_samples) {
    delay_sample_idx_ = current_sample_idx_ + buffer_[0].size() - delay_samples;
  } else {
    delay_sample_idx_ = current_sample_idx_ - delay_samples;
  }
}

void DelayNode::ProduceSamples(const size_t &sample_size,
                               std::vector<std::vector<float>> &output) {
  // TODO: move to constructor
  if (base_audio_context_ptr_.expired()) {
    LOGE("Invalid AudioContext!\n");
    AudioNode::FillWithZeros(sample_size, output);
    return;
  }

  Process(sample_size, output);
}

bool DelayNode::IsBufferValid(const size_t &sample_size,
                              const std::vector<std::vector<float>> &output) {
  if (buffer_.size() < output.size()) {
    return false;
  }
  return true;
}

void DelayNode::MakeBufferInvalid(const size_t channel_size) {
  if (channel_size == 0) {
    return;
  }
  if (buffer_.size() < channel_size) {
    buffer_.resize(channel_size);
  }

  const uint32_t buffer_size =
      std::ceil((delay_time_ref_->max_value() + 1) * sample_rate_);
  if (buffer_[0].size() < buffer_size) {
    for (auto &channel : buffer_) {
      channel.resize(buffer_size);
    }
  }
}

void DelayNode::WriteOutputToBuffer(
    const size_t &sample_size, const std::vector<std::vector<float>> &output) {
  if (sample_size > sample_rate_) {
    LOGE("Sample size is large than sample rate, delay node cannot handle this "
         "situation!\n");
    return;
  }
  if (buffer_.size() < output.size()) {
    LOGE("FATAL! Cannot write output to buffer, channel of buffer is less than "
         "output!\n");
    return;
  }
  for (int i = 0; i < output.size(); i++) {
    if (output[i].size() < sample_size) {
      LOGE("FATAL! Invalid output, output size is less than request sample "
           "size!\n");
      return;
    }

    // TODO: try to optimize here
    const size_t buffer_channel_size = buffer_[i].size();
    uint32_t curr_idx = current_sample_idx_;
    for (int j = 0; j < sample_size; j++, curr_idx++) {
      if (curr_idx >= buffer_channel_size) {
        curr_idx = curr_idx % buffer_channel_size;
      }
      buffer_[i][curr_idx] = output[i][j];
    }
  }
  current_sample_idx_ = (current_sample_idx_ + sample_size) % buffer_[0].size();
}

void DelayNode::ReadBufferToOutput(const size_t &sample_size,
                                   std::vector<std::vector<float>> &output) {
  if (!IsBufferValid(sample_size, output)) {
    LOGE("Inner buffer is not ready!\n");
    return;
  }
  if (sample_size > sample_rate_) {
    LOGE("Sample size is large than sample rate, delay node cannot handle this "
         "situation!\n");
    return;
  }

  for (uint32_t i = 0; i < output.size(); i++) {
    uint32_t delay_idx = delay_sample_idx_;
    for (uint32_t j = 0; j < sample_size; j++, delay_idx++) {
      const uint32_t buffer_channel_size = buffer_[i].size();
      if (delay_idx >= buffer_channel_size) {
        delay_idx = delay_idx % buffer_channel_size;
      }
      output[i][j] = buffer_[i][delay_idx];
    }
  }
  delay_sample_idx_ = (delay_sample_idx_ + sample_size) % buffer_[0].size();
}

void DelayNode::ConnectTo(std::shared_ptr<AudioNode> dst_audio_node_ref) {
  if (IsSelfPtr(dst_audio_node_ref)) {
    LOGE("Cannot connect to self!\n");
    return;
  } else if (!IsSameContext(dst_audio_node_ref)) {
    LOGE("Error! Cannot connect to audio node which is created by other audio "
         "context !\n");
    return;
  }
  std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
  dst_audio_node_ptr_ = dst_audio_node_ref;
}

void DelayNode::BeConnectedTo(std::shared_ptr<AudioNode> src_audio_node_ref) {
  if (IsSelfPtr(src_audio_node_ref)) {
    LOGE("Cannot be connected to self!\n");
    return;
  } else if (!IsSameContext(src_audio_node_ref)) {
    LOGE("Error! Cannot be connected to audio node which is created by other "
         "audio context !\n");
    return;
  }
  std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
  src_audio_node_ref_ = src_audio_node_ref;
}

void DelayNode::Disconnect() {
  if (auto dst_audio_node_ref = dst_audio_node_ptr_.lock()) {
    dst_audio_node_ref->BeDisconnected(*this);
  }
  std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
  dst_audio_node_ptr_.reset();
}

void DelayNode::BeDisconnected(const AudioNode &audio_node) {
  if (&audio_node != src_audio_node_ref_.get()) {
    LOGE("BeDisconnected failed! Parameter AudioNode is different from current "
         "source AudioNode.\n");
    return;
  }

  std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
  src_audio_node_ref_.reset();
}

bool DelayNode::IsValidMaxDelayTime(const float &max_delay_time) {
  if (max_delay_time <= kMinOfMaxDelayTime ||
      max_delay_time >= kMaxOfMaxDelayTime) {
    return false;
  }
  return true;
}

DelayNode::DelayNode(const DelayOptions &options, const float &sample_rate,
                     std::shared_ptr<std::mutex> audio_context_lock_ref)
    : AudioNode(kNumberOfInputs, kNumberOfOutputs, options,
                audio_context_lock_ref),
      delay_time_value_(options.delay_time), sample_rate_(sample_rate) {

  if (buffer_.size() < channel_count_) {
    buffer_.resize(channel_count_);
  }
  for (auto &channel : buffer_) {
    channel.resize(options.max_delay_time, 0);
  }
  delay_sample_idx_ = current_sample_idx_ = 0;
}

}; // namespace js_audio