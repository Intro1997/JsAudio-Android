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
  auto audio_engine_ptr = js_audio::AudioEngine::Get();
  if (!audio_engine_ptr.expired()) {
    audio_engine_ptr.lock()->Start();
  } else {
    LOGE("Start audio engine failed! Audio engine is invalid!\n");
  }
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_audio_AudioHandler_pauseNativeAudioEngine(JNIEnv *env,
                                                           jobject thiz) {
  auto audio_engine_ptr = js_audio::AudioEngine::Get();
  if (!audio_engine_ptr.expired()) {
    audio_engine_ptr.lock()->Pause();
  } else {
    LOGE("Pause audio engine failed! Audio engine is invalid!\n");
  }
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_audio_AudioHandler_resumeNativeAudioEngine(JNIEnv *env,
                                                            jobject thiz) {
  auto audio_engine_ptr = js_audio::AudioEngine::Get();
  if (!audio_engine_ptr.expired()) {
    audio_engine_ptr.lock()->Resume();
  } else {
    LOGE("Resume audio engine failed! Audio engine is invalid!\n");
  }
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_audio_AudioHandler_stopNativeAudioEngine(JNIEnv *env,
                                                          jobject thiz) {
  auto audio_engine_ptr = js_audio::AudioEngine::Get();
  if (!audio_engine_ptr.expired()) {
    audio_engine_ptr.lock()->Stop();
  } else {
    LOGE("Stop audio engine failed! Audio engine is invalid!\n");
  }
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_audio_AudioHandler_destroyNativeAudioEngine(JNIEnv *env,
                                                             jobject thiz) {
  auto audio_engine_ptr = js_audio::AudioEngine::Get();
  if (!audio_engine_ptr.expired()) {
    audio_engine_ptr.lock()->Destroy();
  } else {
    LOGE("Destroy audio engine failed! Audio engine is invalid!\n");
  }
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_audio_AudioHandler_setAudioEngineInitParam(
    JNIEnv *env, jobject thiz, jint output_sample_rate_hz) {

  auto audio_player_config = js_audio::AudioEngine::audio_player_config();

  audio_player_config.sample_rate_milli_hz = output_sample_rate_hz * 1e3;

  js_audio::AudioEngine::set_audio_player_config(audio_player_config);
}