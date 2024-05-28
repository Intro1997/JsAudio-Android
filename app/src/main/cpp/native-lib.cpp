#include "logger.hpp"
#include "node_init.hpp"
#include <jni.h>
#include <node/node.h>
#include <node/uv.h>
#include <string>

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_jsaudio_MainActivity_stringFromJNI(JNIEnv *env,
                                                    jobject /* this */) {
  std::string node_init_ret = "Init Node failed";

  int ret = InitNode();
  if (ret == 0) {
    node_init_ret = "Init Node success!";
  }
  return env->NewStringUTF(node_init_ret.c_str());
}