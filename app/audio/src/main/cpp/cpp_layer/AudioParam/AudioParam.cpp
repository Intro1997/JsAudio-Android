#include "AudioParam.hpp"
#include "logger.hpp"
#include <functional>

namespace js_audio {

const std::string AudioParam::A_RATE = "a-rate";
const std::string AudioParam::K_RATE = "k-rate";
AudioParam::AudioParam(const std::string &node_property_name,
                       const std::string &automation_rate,
                       const float &default_value, const float &min_value,
                       const float &max_value,
                       std::shared_ptr<std::mutex> audio_context_lock_ref,
                       std::function<SetterCallbackFunc> setter_cb,
                       std::function<GetterCallbackFunc> getter_cb)
    : AudioParam(node_property_name, default_value, automation_rate,
                 default_value, min_value, max_value, audio_context_lock_ref,
                 setter_cb, getter_cb) {}

AudioParam::AudioParam(const std::string &node_property_name,
                       const float &value, const std::string &automation_rate,
                       const float &default_value, const float &min_value,
                       const float &max_value,
                       std::shared_ptr<std::mutex> audio_context_lock_ref,
                       std::function<SetterCallbackFunc> setter_cb,
                       std::function<GetterCallbackFunc> getter_cb)
    : automation_rate_(automation_rate), default_value_(default_value),
      min_value_(min_value), max_value_(max_value),
      audio_context_lock_ref_(audio_context_lock_ref),
      setter_cb_(std::move(setter_cb)), getter_cb_(std::move(getter_cb)),
      node_property_name_(node_property_name) {
  value_ = std::clamp(value, min_value, max_value);
  if (value_ != value && setter_cb) {
    setter_cb(value_);
  }
}

void AudioParam::set_value(const float &value) {
  {
    std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
    value_ = std::clamp(value, min_value_, max_value_);
    if (value_ != value) {
      LOGW("%s.value %f outside nominal range [-%f, %f]; value will be "
           "clamped.\n",
           node_property_name_.c_str(), value, min_value_, max_value_);
    }
  }

  if (setter_cb_) {
    setter_cb_(value_);
  }
}

const float AudioParam::value() const {
  if (getter_cb_) {
    getter_cb_();
  }

  std::lock_guard<std::mutex> guard(*audio_context_lock_ref_);
  return value_;
}

const std::string &AudioParam::automation_rate() const {
  return automation_rate_;
}

const float AudioParam::default_value() const { return default_value_; }

const float AudioParam::min_value() const { return min_value_; }

const float AudioParam::max_value() const { return max_value_; }

} // namespace js_audio
