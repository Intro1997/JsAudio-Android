#include "node_http_require.hpp"
#include "logger.hpp"
#include "node_jni_env.hpp"
#include <string>

namespace node_http_require {

static jobject CreateJniObjectFromString(JNIEnv *jni_env, const char *str) {
  return jni_env->NewStringUTF(str);
}

static std::string ConvertJObjectToString(JNIEnv *jni_env, jobject obj) {
  return jni_env->GetStringUTFChars((jstring)obj, nullptr);
}

static std::string loadFileContentFromJsEntry(const char *url) {
  std::string ret = "";

  // loadFileContentFromJsEntry must be called in JS Thread, so we pass false here
  JniEnvWrapper jni_env_wrapper(NodeJniVm::GetJniVm(), false);
  JNIEnv *jni_env = jni_env_wrapper.env();

  if (!jni_env) {
    LOGE("Invalid JNI env!\n");
    return ret;
  }

  jclass clazz = jni_env->FindClass("com/example/node_env/NodeEnvHandler");
  if (!clazz) {
    LOGE("FindClass com/example/node_env/NodeEnvHandler failed!\n");
    return ret;
  }
  jmethodID method_id = jni_env->GetStaticMethodID(
      clazz, "loadFileFromJsEntry", "(Ljava/lang/String;)Ljava/lang/String;");

  if (!method_id) {
    LOGE("GetStaticMethodID loadFileFromJsEntry failed!\n");
    return ret;
  }

  jvalue jargs[1];
  jargs[0].l = CreateJniObjectFromString(jni_env, url);

  jobject jret = jni_env->CallStaticObjectMethodA(clazz, method_id, jargs);

  ret = ConvertJObjectToString(jni_env, jret);

  jni_env->DeleteLocalRef(jargs[0].l);
  jni_env->DeleteLocalRef(jret);

  return ret;
}

static void
loadFileContentFromJsEntryV8(const v8::FunctionCallbackInfo<v8::Value> &info) {
  if (info.Length() > 0 && info[0]->IsString()) {
    v8::Isolate *isolate = info.GetIsolate();
    v8::String::Utf8Value str_value(isolate, info[0]);
    std::string ret = *str_value;

    ret = node_http_require::loadFileContentFromJsEntry(ret.c_str());

    v8::Local<v8::String> entry_file_code =
        v8::String::NewFromUtf8(isolate, ret.c_str()).ToLocalChecked();

    info.GetReturnValue().Set(entry_file_code);
  }
}

void Init(v8::Local<v8::Object> exports, v8::Local<v8::Value> module,
          v8::Local<v8::Context> context, void *priv) {
  NODE_SET_METHOD(exports, "loadFileContentFromJsEntry", loadFileContentFromJsEntryV8);
}

const char *GetPreloadScript() { return NODE_HTTP_REQUIRE_SCRIPT; }
} // namespace node_http_require
