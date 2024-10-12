#include "WaveProducer.hpp"
#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#define SAWTOOTH_DATA_SET_PATH "data/sawtooth.d"
#define SINE_DATA_SET_PATH "data/sine.d"
#define SQUARE_DATA_SET_PATH "data/square.d"
#define TRIANGLE_DATA_SET_PATH "data/triangle.d"

#define SUCCESS_RET 0
#define ERROR_RET 1

void OutputVector(const std::vector<double> &wave_producer_samples) {
  for (size_t i = 0; i < wave_producer_samples.size(); i += 2) {
    std::cout << wave_producer_samples[i] << "\n";
  }
}

int ValidateData(const std::vector<double> &wave_producer_samples,
                 const char *validation_data_path, size_t pass_error_cnt = 0) {
  std::ifstream validate_data_file(validation_data_path);
  if (!validate_data_file.is_open()) {
    std::cerr << "Open " << validation_data_path << " file failed\n";
    return ERROR_RET;
  }

  std::string line;
  double sample = 0.0;
  size_t wave_producer_sample_idx = 0;
  double abs_diff;
  double threshold = 0.01f;
  while (std::getline(validate_data_file, line)) {
    try {
      sample = std::stof(line);
    } catch (const std::exception &e) {
      printf("Error! Validation data file[%s] has non-number line[%s]!\n",
             validation_data_path, line.c_str());
      return ERROR_RET;
    }

    abs_diff = std::abs(
        sample - (double)wave_producer_samples[wave_producer_sample_idx]);
    if (abs_diff > threshold) {
      std::cout << "Validate with file " << validation_data_path
                << " failed:\n";
      printf("\t wave_producer_samples[%zu]<%f> != sample<%f>, "
             "origin_line<%s>, abs diff is %f\n",
             wave_producer_sample_idx,
             wave_producer_samples[wave_producer_sample_idx], sample,
             line.c_str(), abs_diff);
      if (!pass_error_cnt) {
        return ERROR_RET;
      }
      pass_error_cnt--;
    }
    wave_producer_sample_idx += 2;
  }

  validate_data_file.close();
  return 0;
}

int ValidateWaveData(const double &freq, const double &sample_rate,
                     const double &sign, const size_t &sample_size) {
  const double step = freq * 2 * M_PI / sample_rate;
  double current_time = 0;
  double scale = 1.0;
  std::vector<double> wave_producer_samples;
  wave_producer_samples.resize(sample_size, 0);
  // Avoiding Errors from Discontinuous Functions
  size_t pass_error_cnt = freq;
  if (freq == 1) {
    pass_error_cnt = freq * 2;
  }

  // validate sine wave
  js_audio::WaveProducer::CreateSineWave<double>(
      step, current_time, sign, sample_size, wave_producer_samples, scale);
  if (ValidateData(wave_producer_samples, SINE_DATA_SET_PATH) == ERROR_RET) {
    return ERROR_RET;
  }
  std::fill(wave_producer_samples.begin(), wave_producer_samples.end(), 0);
  current_time = 0;

  // validate sawtooth wave
  js_audio::WaveProducer::CreateSawtoothWave<double>(
      step, current_time, sign, sample_size, wave_producer_samples, scale);
  if (ValidateData(wave_producer_samples, SAWTOOTH_DATA_SET_PATH,
                   pass_error_cnt) == ERROR_RET) {
    return ERROR_RET;
  }

  std::fill(wave_producer_samples.begin(), wave_producer_samples.end(), 0);
  current_time = 0;

  // validate square wave
  js_audio::WaveProducer::CreateSquareWave<double>(
      step, current_time, sign, sample_size, wave_producer_samples, scale);
  if (ValidateData(wave_producer_samples, SQUARE_DATA_SET_PATH,
                   pass_error_cnt) == ERROR_RET) {
    return ERROR_RET;
  }
  std::fill(wave_producer_samples.begin(), wave_producer_samples.end(), 0);
  current_time = 0;

  // validate triangle wave
  js_audio::WaveProducer::CreateTriangleWave<double>(
      step, current_time, sign, sample_size, wave_producer_samples, scale);
  if (ValidateData(wave_producer_samples, TRIANGLE_DATA_SET_PATH) ==
      ERROR_RET) {
    return ERROR_RET;
  }

  return 0;
}

int main(int argc, char **argv) {
  if (argc < 5) {
    std::cout << "Need 4 parameter<frequency, sampleRate, sign, sampleSize> at "
                 "least, but get "
              << argc - 1 << " only!\n";
    return ERROR_RET;
  }

  double freq = 0, sample_rate = 0, sign = 1.0;
  size_t sample_size = 0.0;
  try {
    freq = std::stof(argv[1]);
    sample_rate = std::stof(argv[2]);
    sign = std::stof(argv[3]);
    sample_size = std::stof(argv[4]);
  } catch (const std::invalid_argument &ia) {
    std::cout << "Invalid parameter, parameter must be number!\n";
  } catch (const std::out_of_range &oor) {
    std::cout << "Invalid parameter, parameter out of range!\n";
  }

  sign = sign > 0.0 ? 1.0 : -1.0;

  return ValidateWaveData(freq, sample_rate, sign, sample_size);
}