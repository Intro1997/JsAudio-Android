#ifndef TIMER_HPP_
#define TIMER_HPP_
#include <chrono>
#include <cmath>

enum class TimeUnit { SECOND = 0, MILLISECOND, MICORSECOND, NANOSECOND };

template <typename T1, typename T2>
inline T1 ConvertTimeToType(T2 duration, const TimeUnit &unit) {
  switch (unit) {
  case TimeUnit::SECOND: {
    return std::chrono::duration_cast<std::chrono::seconds>(duration).count();
  }
  case TimeUnit::MILLISECOND: {
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration)
        .count();
  }
  case TimeUnit::MICORSECOND: {
    return std::chrono::duration_cast<std::chrono::microseconds>(duration)
        .count();
  }
  case TimeUnit::NANOSECOND: {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(duration)
        .count();
  }
  }
  return (T1)0;
}

template <typename Ret> inline Ret GetCurrentSystemTime(const TimeUnit &unit) {
  return ConvertTimeToType<Ret, std::chrono::system_clock::duration>(
      std::chrono::system_clock::now().time_since_epoch(), unit);
}

template <TimeUnit left, TimeUnit right>
inline double ConvertFromLeftTimeUnitToRight(const double &time_val) {
  int32_t left_int_val = static_cast<int32_t>(left);
  int32_t right_int_val = static_cast<int32_t>(right);
  int32_t right_left_diff = right_int_val - left_int_val;

  double ret = time_val;

  if (right_left_diff != 0) {
    ret = time_val * std::pow(1e3, right_left_diff);
  }
  
  return ret;
}

#endif // TIMER_HPP_