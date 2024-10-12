#ifndef NODE_KT_BRIDGE_HPP_
#define NODE_KT_BRIDGE_HPP_

#include "logger.hpp"
#include "pthread.h"
#include <jni.h>
#include <string>

class JniEnvWrapper {
public:
  inline JniEnvWrapper(JavaVM *vm, bool need_detach)
      : vm_(vm), env_(nullptr), need_detach_(need_detach) {}

  inline JNIEnv *env() {
    env_ = nullptr;
    if (vm_) {
      int status = vm_->GetEnv((void **)env_, JNI_VERSION_1_6);
      if (status == JNI_EDETACHED || env_ == nullptr) {
        status = vm_->AttachCurrentThread(&env_, nullptr);
        if (status != JNI_OK) {
          env_ = nullptr;
          LOGE("CallStaticStringMethod failed, get jni env failed [ErrorCode: "
               "%d]\n",
               status);
        }
      }
    } else {
      LOGE("Invalid JVM pointer!\n");
    }
    return env_;
  }

  inline ~JniEnvWrapper() {
    if (need_detach_) {
      vm_->DetachCurrentThread();
      need_detach_ = false;
    }
    env_ = nullptr;
  }

private:
  JNIEnv *env_;
  JavaVM *vm_;
  // need_detach_: control if do DetachCurrentThread() after
  // finish java function call.
  // You SHOULD NOT call DetachCurrentThread in the thread
  // born from java/kotlin.
  // You SHOULD call DetachCurrentThread in thread not born
  // from java/kotlin.
  bool need_detach_;
};

class NodeJniVm {
public:
  static void Init(JavaVM *vm);
  static JavaVM *GetJniVm();

private:
  static JavaVM *vm_;
};

inline JavaVM *NodeJniVm::vm_ = nullptr;

inline void NodeJniVm::Init(JavaVM *vm) { vm_ = vm; }

inline JavaVM *NodeJniVm::GetJniVm() { return vm_; }

#endif // NODE_KT_BRIDGE_HPP