#include <jni.h>
#include "preload_script.hpp"

//
// Created by intro on 2024/6/11.
//

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_audio_AudioHandler_getPreloadScriptFromNative(JNIEnv *env, jobject thiz) {
  return env->NewStringUTF(AUDIO_PRELOAD_SCRIPT);
}