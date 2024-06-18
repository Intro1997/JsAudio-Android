#include "AudioEngine.hpp"
#include "logger.hpp"
#include "preload_script.hpp"
#include <jni.h>

//
// Created by intro on 2024/6/11.
//

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
