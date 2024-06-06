#include "NodeInstance.hpp"
#include "logger.hpp"
#include <jni.h>
#include <node/node.h>
#include <node/uv.h>
#include <string>

static NodeInstance *global_node = nullptr;

extern "C" JNIEXPORT jboolean JNICALL
Java_com_example_nodeenvlib_NodeEnvHandle_createNativeNode(JNIEnv *env,
                                                           jobject thiz) {
  LOGD("Get in node env handle native function\n");
  global_node = NodeInstance::Create();
  if (!global_node) {
    LOGE("Create global node failed\n");
    delete global_node;
    global_node = nullptr;
    return false;
  }
  return true;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_nodeenvlib_NodeEnvHandle_nativeEvalCode(JNIEnv *env, jobject thiz,
                                                         jstring code_str) {
  jboolean is_copy;
  const char *code_c_str = env->GetStringUTFChars(code_str, &is_copy);
  std::string code = code_c_str;
  std::string result;
  if (!global_node->Eval(code, result)) {
    LOGE("Eval code error: \n\tsrc: %s\n\tmsg: %s\n", code.c_str(),
         result.c_str());
  }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_nodeenvlib_NodeEnvHandle_destroyNativeNode(JNIEnv *env, jobject thiz) {
  if (global_node) {
    global_node->Destroy();
    delete global_node;
  }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_nodeenvlib_NodeEnvHandle_pauseNativeNode(JNIEnv *env, jobject thiz) {
  if (global_node) {
    global_node->Pause();
  } else {
    LOGW("Pause node failed, global node instance is invalid\n");
  }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_nodeenvlib_NodeEnvHandle_resumeNativeNode(JNIEnv *env, jobject thiz) {
  if (global_node) {
    global_node->Resume();
  } else {
    LOGW("Resume node failed, global node instance is invalid\n");
  }
}