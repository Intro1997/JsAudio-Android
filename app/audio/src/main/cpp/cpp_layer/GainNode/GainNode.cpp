#include "GainNode.hpp"
#include "logger.hpp"

namespace js_audio {
using GainNodeOptions = GainNode::GainNodeOptions;
using ChannelCountMode = AudioNode::ChannelCountMode;
using ChannelInterpretation = AudioNode::ChannelInterpretation;

const uint32_t GainNode::kNumberOfInputs = 1;
const uint32_t GainNode::kNumberOfOutputs = 1;
const float GainNode::kDefaultGain = 1.0f;

class GainNodeConstructHelper : public GainNode {
public:
  GainNodeConstructHelper(const GainNodeOptions &options,
                          std::shared_ptr<std::mutex> audio_context_lock_ref)
      : GainNode(options, audio_context_lock_ref) {}
};

std::shared_ptr<GainNode>
GainNode::CreateGainNode(std::shared_ptr<std::mutex> audio_context_lock_ref) {
  GainNodeOptions options = GetDefaultOptions();
  return CreateGainNode(options, audio_context_lock_ref);
}

std::shared_ptr<GainNode>
GainNode::CreateGainNode(const GainNodeOptions &options,
                         std::shared_ptr<std::mutex> audio_context_lock_ref) {
  std::shared_ptr<GainNode> gain_node_ref =
      std::make_shared<GainNodeConstructHelper>(options,
                                                audio_context_lock_ref);

  std::weak_ptr<GainNode> gain_node_ptr = gain_node_ref;
  std::function<AudioParam::SetterCallbackFunc> setter_cb =
      [=](const float &gain_value) {
        if (auto gain_node_ref = gain_node_ptr.lock()) {
          gain_node_ref->set_gain_value(gain_value);
        }
      };

  gain_node_ref->gain_ref_ = std::make_shared<AudioParam>(
      options.gain, AudioParam::A_RATE, kDefaultGain, FLT_MIN, FLT_MAX,
      audio_context_lock_ref, setter_cb);
  return gain_node_ref;
}

GainNode::GainNode(const GainNodeOptions &options,
                   std::shared_ptr<std::mutex> audio_context_lock_ref)
    : AudioNode(kNumberOfInputs, kNumberOfOutputs, options,
                audio_context_lock_ref),
      gain_value_(options.gain) {}

void GainNode::set_gain_value(const float &gain_value) {
  std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
  gain_value_ = gain_value;
}

GainNodeOptions GainNode::GetDefaultOptions() {
  GainNodeOptions options;
  options.channel_count = 2;
  options.channel_count_mode = ChannelCountMode::kMax;
  options.channel_interpretation = ChannelInterpretation::kSpeakers;
  options.gain = 1.0f;
  return options;
}

std::shared_ptr<AudioParam> GainNode::gain_ref() const { return gain_ref_; }

void GainNode::ConnectTo(std::shared_ptr<AudioNode> dst_audio_node_ptr) {
  if (IsSelfPtr(dst_audio_node_ptr)) {
    LOGE("Error! Cannot connect to self!\n");
    return;
  }
  std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
  dst_audio_node_ptr_ = dst_audio_node_ptr;
}

void GainNode::BeConnectedTo(std::shared_ptr<AudioNode> src_audio_node_ptr) {
  if (IsSelfPtr(src_audio_node_ptr)) {
    LOGE("Error! Cannot be connected by self!\n");
    return;
  }
  std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
  src_audio_node_ref_ = src_audio_node_ptr;
}

void GainNode::Disconnect() {
  if (auto dst_audio_node_ref = dst_audio_node_ptr_.lock()) {
    dst_audio_node_ref->BeDisconnected(*this);
  }
  std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
  dst_audio_node_ptr_.reset();
}

void GainNode::BeDisconnected(const AudioNode &audio_node) {
  if (&audio_node != src_audio_node_ref_.get()) {
    LOGE("BeDisconnected failed! Parameter AudioNode is different from current "
         "source AudioNode.\n");
    return;
  }

  std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
  src_audio_node_ref_.reset();
}
void GainNode::ProduceSamples(const size_t &sample_size,
                              std::vector<std::vector<float>> &output) {
  if (src_audio_node_ref_) {
    src_audio_node_ref_->ProduceSamples(sample_size, output);
    ProcessSamples(sample_size, output);
  } else {
    AudioNode::FillWithZeros(sample_size, output);
  }
}

void GainNode::ProcessSamples(const size_t &sample_size,
                              std::vector<std::vector<float>> &outputs) {
  // TODO: make optimization here
  for (auto &channel : outputs) {
    for (size_t i = 0; i < channel.size(); i++) {
      channel[i] *= gain_value_;
    }
  }
}

} // namespace js_audio