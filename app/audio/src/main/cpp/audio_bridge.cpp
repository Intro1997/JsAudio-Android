#include "AudioEngine.hpp"
#include "logger.hpp"
#include "preload_script.hpp"
#include <jni.h>

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_audio_AudioHandler_getPreloadScriptFromNative(JNIEnv *env,
                                                               jobject thiz) {
  return env->NewStringUTF(AUDIO_PRELOAD_SCRIPT);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_audio_AudioHandler_startNativeAudioEngine(JNIEnv *env,
                                                           jobject thiz) {
  auto weak_audio_engine = js_audio::AudioEngine::Get();
  if (!weak_audio_engine.expired()) {
    weak_audio_engine.lock()->Start();
  } else {
    LOGE("Start audio engine failed! Audio engine is invalid!\n");
  }
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_audio_AudioHandler_pauseNativeAudioEngine(JNIEnv *env,
                                                           jobject thiz) {
  auto weak_audio_engine = js_audio::AudioEngine::Get();
  if (!weak_audio_engine.expired()) {
    weak_audio_engine.lock()->Pause();
  } else {
    LOGE("Pause audio engine failed! Audio engine is invalid!\n");
  }
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_audio_AudioHandler_resumeNativeAudioEngine(JNIEnv *env,
                                                            jobject thiz) {
  auto weak_audio_engine = js_audio::AudioEngine::Get();
  if (!weak_audio_engine.expired()) {
    weak_audio_engine.lock()->Resume();
  } else {
    LOGE("Resume audio engine failed! Audio engine is invalid!\n");
  }
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_audio_AudioHandler_stopNativeAudioEngine(JNIEnv *env,
                                                          jobject thiz) {
  auto weak_audio_engine = js_audio::AudioEngine::Get();
  if (!weak_audio_engine.expired()) {
    weak_audio_engine.lock()->Stop();
  } else {
    LOGE("Stop audio engine failed! Audio engine is invalid!\n");
  }
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_audio_AudioHandler_destroyNativeAudioEngine(JNIEnv *env,
                                                             jobject thiz) {
  auto weak_audio_engine = js_audio::AudioEngine::Get();
  if (!weak_audio_engine.expired()) {
    weak_audio_engine.lock()->Destroy();
  } else {
    LOGE("Destroy audio engine failed! Audio engine is invalid!\n");
  }
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_audio_AudioHandler_setAudioEngineInitParam(
    JNIEnv *env, jobject thiz, jint output_sample_rate_hz,
    jint output_frames_per_buffer) {

  auto audio_player_config = js_audio::AudioEngine::audio_player_config();

  audio_player_config.sample_rate_milli_hz = output_sample_rate_hz * 1e3;
  audio_player_config.frames_per_buffer = output_frames_per_buffer;

  js_audio::AudioEngine::set_audio_player_config(audio_player_config);
}