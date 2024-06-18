#define CHECK_WITH_RET(left, right, ret_value)                                 \
  do {                                                                         \
    if (left == right) {                                                       \
      return ret_value;                                                        \
    }                                                                          \
  } while (false)

#define CHECK_WITH_RET_CALL(left, right, ret_value, func_call)            \
  do {                                                                         \
    if (!((left) == (right))) {                                                \
      func_call;                                                               \
      return ret_value;                                                        \
    }                                                                          \
  } while (false)
