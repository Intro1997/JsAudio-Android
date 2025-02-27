#ifndef JS_AUDIO_WAVE_PRODUCER_
#define JS_AUDIO_WAVE_PRODUCER_

#include <cmath>
#include <vector>

#define DOUBLE_M_PI (M_PI * 2)
#define TRIBLE_PI_2 3 * M_PI_2

namespace js_audio {
class WaveProducer {
public:
  enum class WaveType {
    kSine = 0,
    kSawtooth,
    kSquare,
    kTriangle,
  };
  template <typename T>
  static void CreateSineWave(const double &step, double &current_time,
                             const double &sign, const size_t &sample_size,
                             std::vector<T> &output, const T &scaler);
  template <typename T>
  static void CreateSawtoothWave(const double &step, double &current_time,
                                 const double &sign, const size_t &sample_size,
                                 std::vector<T> &output, const T &scaler);
  template <typename T>
  static void CreateSquareWave(const double &step, double &current_time,
                               const double &sign, const size_t &sample_size,
                               std::vector<T> &output, const T &scaler);
  template <typename T>
  static void CreateTriangleWave(const double &step, double &current_time,
                                 const double &sign, const size_t &sample_size,
                                 std::vector<T> &output, const T &scaler);
};
static const double kSawtoothWaveKParam = 1.0f / M_PI;
static const double kTriangleWaveKParam = 2.0f / M_PI;

template <typename T>
void WaveProducer::CreateSineWave(const double &step, double &current_time,
                                  const double &sign, const size_t &sample_size,
                                  std::vector<T> &output, const T &scaler) {

  if (step == 0.0) {
    std::fill(output.begin(), output.end(), 0);
    return;
  }

  output.resize(sample_size, 0);
  for (size_t i = 0; i < sample_size; i++) {
    output[i] = (T)(sign * std::sin(current_time) * scaler);
    current_time += step;
    if (current_time > DOUBLE_M_PI) {
      current_time -= DOUBLE_M_PI;
    }
  }
}

template <typename T>
void WaveProducer::CreateSawtoothWave(const double &step, double &current_time,
                                      const double &sign,
                                      const size_t &sample_size,
                                      std::vector<T> &output, const T &scaler) {
  if (step == 0.0) {
    std::fill(output.begin(), output.end(), 0);
    return;
  }

  output.resize(sample_size, 0);
  for (size_t i = 0; i < sample_size; i++) {
    output[i] = (T)(sign * kSawtoothWaveKParam * current_time * scaler);
    current_time += step;
    if (current_time > M_PI) {
      current_time -= DOUBLE_M_PI;
    }
  }
}

template <typename T>
void WaveProducer::CreateSquareWave(const double &step, double &current_time,
                                    const double &sign,
                                    const size_t &sample_size,
                                    std::vector<T> &output, const T &scaler) {
  if (step == 0.0) {
    std::fill(output.begin(), output.end(), 0);
    return;
  }

  output.resize(sample_size, 0);
  const T left_value = sign * scaler;
  const T right_value = -sign * scaler;

  for (size_t i = 0; i < sample_size; i++) {
    output[i] = current_time <= M_PI ? left_value : right_value;

    current_time += step;
    if (current_time > DOUBLE_M_PI) {
      current_time -= DOUBLE_M_PI;
    }
  }
}

template <typename T>
void WaveProducer::CreateTriangleWave(const double &step, double &current_time,
                                      const double &sign,
                                      const size_t &sample_size,
                                      std::vector<T> &output, const T &scaler) {
  if (step == 0.0) {
    std::fill(output.begin(), output.end(), 0);
    return;
  }

  output.resize(sample_size, 0);
  for (size_t i = 0; i < sample_size; i++) {
    if (current_time <= M_PI_2) {
      output[i] = sign * kTriangleWaveKParam * current_time * scaler;
    } else if (current_time > TRIBLE_PI_2) {
      output[i] =
          sign * kTriangleWaveKParam * (current_time - DOUBLE_M_PI) * scaler;
    } else {
      output[i] = sign * -kTriangleWaveKParam * (current_time - M_PI) * scaler;
    }

    current_time += step;
    if (current_time > DOUBLE_M_PI) {
      current_time -= DOUBLE_M_PI;
    }
  }
}

} // namespace js_audio

#endif