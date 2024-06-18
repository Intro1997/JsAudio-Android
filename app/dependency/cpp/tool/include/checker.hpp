#pragma once
#define CHECK_WITH_RET(situation, ret_value)                                   \
  do {                                                                         \
    if (!(situation)) {                                                        \
      return ret_value;                                                        \
    }                                                                          \
  } while (false)

#define CHECK_WITH_RET_CALL(situation, ret_value, func_call)                   \
  do {                                                                         \
    if (!(situation)) {                                                        \
      func_call;                                                               \
      return ret_value;                                                        \
    }                                                                          \
  } while (false)
