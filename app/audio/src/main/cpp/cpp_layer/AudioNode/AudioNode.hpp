#pragma once
#include <string>
namespace js_audio {
class AudioNode {
public:
  AudioNode() = default;

  uint32_t number_of_inputs() const;
  uint32_t number_of_outputs() const;
  uint32_t channel_count() const;
  std::string channel_count_mode() const;

protected:
  void set_number_of_inputs(const uint32_t &number_of_inputs);
  void set_number_of_outputs(const uint32_t &number_of_outputs);
  void set_channel_count(const uint32_t &channel_count);
  void set_channel_count_mode(const std::string &channel_count_mode);

private:
  uint32_t number_of_inputs_;
  uint32_t number_of_outputs_;
  uint32_t channel_count_;
  std::string channel_count_mode_;


};
} // namespace js_audio