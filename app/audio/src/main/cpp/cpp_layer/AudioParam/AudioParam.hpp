#pragma once
#include <mutex>
#include <string>

namespace js_audio {
class AudioParam {
public:
  using SetterCallbackFunc = void(const float &);
  using GetterCallbackFunc = void();

  AudioParam(const std::string &automation_rate, const float &default_value,
             const float &min_value, const float &max_value,
             std::shared_ptr<std::mutex> audio_context_lock_ref,
             std::function<SetterCallbackFunc> setter_cb = nullptr,
             std::function<GetterCallbackFunc> getter_cb = nullptr);

  void set_value(const float &value);

  const float value() const;
  const std::string &automation_rate() const;
  const float default_value() const;
  const float min_value() const;
  const float max_value() const;

  static const std::string A_RATE;
  static const std::string K_RATE;

private:
  float value_;
  const std::string automation_rate_;
  const float default_value_;
  const float min_value_;
  const float max_value_;
  std::shared_ptr<std::mutex> audio_context_lock_ref_;

  std::function<SetterCallbackFunc> setter_cb_;
  std::function<GetterCallbackFunc> getter_cb_;
};
} // namespace js_audio
