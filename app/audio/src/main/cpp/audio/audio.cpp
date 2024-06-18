
#include <jni.h>
#include <node/node_api.h>
#include <string>

#include "AudioBufferQueuePlayer.hpp"
#include "AudioEngine.hpp"
#include "audio.hpp"
#include "logger.hpp"
#include "preload_script.hpp"
/*
AduioEngine
  1. AudioEngine is a singleton design, can only exist one engine at a given
time
  2. AudioEngine has thread-safe mode and thread-unsafe mode, can be specified
at engine object creation.
  3. AudioEngine uses SLThreadSyncItf to make thread-safe. But you should create
thread-safe AudioEngine first!
  4. You can query implementation-specific capabilities by AudioEngine

AudioMedia
  1. Types: player, recorder, etc.
  2. Defined by process operation, input data type and output data type
  3. Input data is located in data source;
  4. Output data is sended to data sink;
  5. Data is defined by FORMAT(PCM or other) and LOCATION(Buffer, file or other)

*/

namespace js_audio {
const char *GetPreLoadScript() { return AUDIO_PRELOAD_SCRIPT; }
} // namespace js_audio

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
  std::weak_ptr<js_audio::AudioEngine> audio_engine =
      js_audio::AudioEngine::Get();

  if (!audio_engine.expired()) {
    auto audio_engine_lock = audio_engine.lock();
    audio_engine_lock->CreateAudioBufferQueuePlayer(
        4, SL_DATAFORMAT_PCM, 2, SL_SAMPLINGRATE_44_1,
        SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
        SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
        SL_BYTEORDER_LITTLEENDIAN, SL_DATALOCATOR_OUTPUTMIX);
    std::weak_ptr<js_audio::AudioPlayer> audio_player =
        audio_engine_lock->GetAudioPlayer();
    audio_player.lock()->Start();
  }

  napi_status status;
  napi_value fn;

  napi_create_function(env, nullptr, 0, AddInt, nullptr, &fn);

  status = napi_set_named_property(env, exports, "add_int", fn);
  if (status != napi_ok)
    return nullptr;

  return exports;
}

NAPI_MODULE(audio, Init);