#include "NodeInstance.hpp"
#include "logger.hpp"
#include <jni.h>
#include <node/node.h>
#include <node/uv.h>
#include <string>

static NodeInstance *global_node;

void init() __attribute__((constructor)) {
  global_node = NodeInstance::Create();
  if (!global_node) {
    LOGE("Create global node failed\n");
    delete global_node;
    global_node = nullptr;
  }
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_jsaudio_MainActivity_getNativeNodeInitState(JNIEnv *env,
                                                             jobject thiz) {
  std::string node_init_ret = "Init Node failed";

  if (global_node != nullptr) {
    node_init_ret = "Init Node success!";
  }

  return env->NewStringUTF(node_init_ret.c_str());
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_jsaudio_MainActivity_destroyNativeNode(JNIEnv *env,
                                                        jobject thiz) {
  if (global_node) {
    global_node->Destroy();
    delete global_node;
  }
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_jsaudio_MainActivity_evalCode(JNIEnv *env, jobject thiz,
                                               jstring code_str) {
  jboolean is_copy;
  const char *code_c_str = env->GetStringUTFChars(code_str, &is_copy);
  std::string code = code_c_str;
  std::string result;
  if (!NodeInstance::Eval(code, result)) {
    LOGE("Eval code error: \n\tsrc: %s\n\tmsg: %s\n", code.c_str(), result.c_str());
  }
}
