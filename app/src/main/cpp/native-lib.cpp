#include "NodeInstance.hpp"
#include "logger.hpp"
#include <jni.h>
#include <node/node.h>
#include <node/uv.h>
#include <string>

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_jsaudio_MainActivity_stringFromJNI(JNIEnv *env,
                                                    jobject /* this */) {
  std::string node_init_ret = "Init Node failed";

  NodeInstance *instance = NodeInstance::Create();
  if (instance != nullptr) {
    node_init_ret = "Init Node success!";
    delete instance;
  }

  return env->NewStringUTF(node_init_ret.c_str());
}