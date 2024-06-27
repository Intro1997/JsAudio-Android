#pragma once
#include <chrono>
#include <thread>

#define sleep_for_ms(ms_time)                                                  \
  do {                                                                         \
    using namespace std::chrono_literals;                                      \
    std::this_thread::sleep_for(ms_time##ms);                                  \
  } while (false)

#define sleep_for_us(us_time)                                                  \
  do {                                                                         \
    using namespace std::chrono_literals;                                      \
    std::this_thread::sleep_for(us_time##us);                                  \
  } while (false)
