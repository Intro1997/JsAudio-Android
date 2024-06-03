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
