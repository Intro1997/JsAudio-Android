#include "NodeEnv.hpp"
#include "logger.hpp"
#include "node_http_require.hpp"
#include "node_jni_env.hpp"
#include "node_logger.hpp"

#include <jni.h>
#include <node/node.h>
#include <node/uv.h>
#include <string>

static NodeEnv *global_node = nullptr;

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
  NodeJniVm::Init(vm);
  return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_example_node_1env_NodeEnvHandler_createNativeNode(
    JNIEnv *env, jobject thiz, jstring preload_script) {
  LOGD("Get in node env handle native function\n");
  jboolean is_copy;
  const char *preload_script_cstr =
      env->GetStringUTFChars(preload_script, &is_copy);

  NodeEnv::AddInternalModule(node_logger::GetPreloadScript(), "node_logger",
                             node_logger::Init);
  NodeEnv::AddInternalModule(node_http_require::GetPreloadScript(),
                             "node_http_require", node_http_require::Init);

  global_node = NodeEnv::Create(preload_script_cstr);
  if (!global_node) {
    LOGE("Create global node failed\n");
    return false;
  }
  return true;
}
extern "C" JNIEXPORT void JNICALL
Java_com_example_node_1env_NodeEnvHandler_nativeEvalCode(JNIEnv *env,
                                                         jobject thiz,
                                                         jstring code_str) {
  jboolean is_copy;
  const char *code_c_str = env->GetStringUTFChars(code_str, &is_copy);
  std::string code = code_c_str;
  std::string result;
  try {
    if (!global_node->Eval(code, result)) {
      LOGE("Eval code error:\n");
      LOGE("src: %s\n", code.c_str());
      LOGE("msg: %s\n", result.c_str());
    }
  } catch (std::exception e) {
  }
}
extern "C" JNIEXPORT void JNICALL
Java_com_example_node_1env_NodeEnvHandler_destroyNativeNode(JNIEnv *env,
                                                            jobject thiz) {
  if (global_node) {
    global_node->Destroy();
    delete global_node;
  }
}
extern "C" JNIEXPORT void JNICALL
Java_com_example_node_1env_NodeEnvHandler_pauseNativeNode(JNIEnv *env,
                                                          jobject thiz) {
  if (global_node) {
    global_node->Pause();
  } else {
    LOGW("Pause node failed, global node instance is invalid\n");
  }
}
extern "C" JNIEXPORT void JNICALL
Java_com_example_node_1env_NodeEnvHandler_resumeNativeNode(JNIEnv *env,
                                                           jobject thiz) {
  if (global_node) {
    global_node->Resume();
  } else {
    LOGW("Resume node failed, global node instance is invalid\n");
  }
}
