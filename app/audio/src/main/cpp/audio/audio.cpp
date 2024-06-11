
#include <jni.h>
#include <node/node_api.h>
#include <string>

#include "audio.hpp"
#include "logger.hpp"
#include "preload_script.hpp"

namespace audio {
const char *GetPreLoadScript() { return AUDIO_PRELOAD_SCRIPT; }
} // namespace audio

static napi_value AddInt(napi_env env, napi_callback_info info) {
  size_t argc = 2;
  napi_value argv[2];
  napi_status status;
  // napi_value
  status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
  if (status != napi_ok) {
    LOGE("[Audio]: napi get value failed1\n");
    return nullptr;
  }

  uint32_t a, b;
  status = napi_get_value_uint32(env, argv[0], &a);
  if (status != napi_ok) {
    LOGE("[Audio]: napi get first parameter failed\n");
    return nullptr;
  }

  status = napi_get_value_uint32(env, argv[1], &b);

  if (status != napi_ok) {
    LOGE("[Audio]: napi get second parameter failed\n");
    return nullptr;
  }

  napi_value ret;
  status = napi_create_uint32(env, a + b, &ret);
  if (status != napi_ok) {
    LOGE("[Audio]: napi create result value failed\n");
    return nullptr;
  }
  return ret;
}

static napi_value Init(napi_env env, napi_value exports) {
  napi_status status;
  napi_value fn;

  napi_create_function(env, nullptr, 0, AddInt, nullptr, &fn);

  status = napi_set_named_property(env, exports, "add_int", fn);
  if (status != napi_ok)
    return nullptr;

  return exports;
}

NAPI_MODULE(audio, Init);