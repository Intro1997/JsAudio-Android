#pragma once
#include "AudioBuffer.hpp"
#include "AudioParam.hpp"
#include "AudioScheduledSourceNode.hpp"
#include <memory>

namespace js_audio {
class AudioBufferSourceNode : public AudioScheduledSourceNode {
public:
  struct AudioBufferSourceOptions {
    std::shared_ptr<AudioBuffer> buffer_ref;
    float detune;
    bool loop;
    double loop_end;
    double loop_start;
    float playback_rate;
  };

  static std::shared_ptr<AudioBufferSourceNode> CreateAudioBufferSourceNode(
      const float &sample_rate,
      std::shared_ptr<std::mutex> audio_context_lock_ref);
  static std::shared_ptr<AudioBufferSourceNode> CreateAudioBufferSourceNode(
      const AudioBufferSourceOptions &options, const float &sample_rate,
      std::shared_ptr<std::mutex> audio_context_lock_ref);
  static AudioBufferSourceOptions GetDefaultOptions();

  static float kDefaultDetune;
  static bool kDefaultLoop;
  static double kDefaultLoopEnd;
  static double kDefaultLoopStart;
  static float kDefaultPlaybackRate;
  static uint32_t kDefaultNumberOfInputs;
  static uint32_t kDefaultNumberOfOutputs;
  static uint32_t kDefaultChannelCount;
  static ChannelCountMode kDefaultChannelCountMode;
  static ChannelInterpretation kDefaultChannelInterpretation;

  // negative duration means use default duration
  void Start(const double &when = 0, const double &offset = 0,
             const double &duration = -1);
  void GetOutputRegionSize(const size_t sample_size,
                           size_t &front_zero_sample_size,
                           size_t &process_sample_size,
                           size_t &back_zero_sample_size);
  bool AssignOutputWithZeros(std::vector<std::vector<float>> &output,
                             const size_t &begin, const size_t &sample_size);
  void ProduceSamples(const size_t &sample_size,
                      std::vector<std::vector<float>> &output) override;
  void Process(std::vector<std::vector<float>> &output, const size_t &begin,
               const size_t &sample_size, const double &actual_loop_start,
               const double &actual_loop_end);
  const std::shared_ptr<const AudioBuffer> audio_buffer_ref() const;
  void set_audio_buffer_ref(std::shared_ptr<AudioBuffer> audio_buffer_ref);

  std::shared_ptr<AudioParam> detune_ref() const;
  void set_detune_ref(const std::shared_ptr<AudioParam> &detune);
  bool loop() const;
  void set_loop(const bool &loop);
  double loop_end() const;
  void set_loop_end(const double &loopEnd);
  double loop_start() const;
  void set_loop_start(const double &loopStart);
  std::shared_ptr<AudioParam> playback_rate_ref() const;
  void set_playback_rate_ref(const std::shared_ptr<AudioParam> &playbackRate);
  float compute_playback_rate() const;
  void UpdateComputePlaybackRate();

protected:
  AudioBufferSourceNode(const AudioBufferSourceOptions &options,
                        const float &sample_rate,
                        std::shared_ptr<std::mutex> audio_context_lock_ref);

private:
  bool GetActualLoopEndingPoint(double &actual_loop_start,
                                double &actual_loop_end);
  bool CanPlay(const double &actual_loop_start, const double &actual_loop_end);
  float GetCurrentTimeOutputSample(const double &actual_loop_start,
                                   const double &actual_loop_end,
                                   const size_t &channel);
  float GetCurrentTimeSampleFromBuffer(const size_t &channel,
                                       const double &actual_loop_start);
  void MakeOutputValid(const size_t &sample_size,
                       std::vector<std::vector<float>> &output);

  std::shared_ptr<AudioBuffer> audio_buffer_ref_;
  std::shared_ptr<AudioParam> detune_ref_;
  bool loop_;
  double loop_end_;
  double loop_start_;
  double current_time_;
  double playing_time_;
  std::shared_ptr<AudioParam> playback_rate_ref_;
  mutable std::mutex compute_playback_rate_lock_;
  float compute_playback_rate_;
  double offset_;
  double duration_;
};
} // namespace js_audio