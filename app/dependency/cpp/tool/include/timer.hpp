#pragma once
#include <chrono>

inline std::chrono::time_point<std::chrono::steady_clock> GetNow() {
  return std::chrono::high_resolution_clock::now();
}

inline double
GetDifference(std::chrono::time_point<std::chrono::steady_clock> start,
              std::chrono::time_point<std::chrono::steady_clock> end) {
  std::chrono::duration<double> duration = end - start;
  return duration.count();
}