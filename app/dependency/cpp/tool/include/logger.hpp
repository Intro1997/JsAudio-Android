//
// Created by Outro on 2023/10/11.
//

#pragma once
#include <android/log.h>
#define ERR_TAG "Native ERROR"
#define WARN_TAG "Native WARNING"
#define DEBUG_TAG "Native DEBUG"
#define INFO_TAG "Native INFO"

// avoid c/c++ extension bug, still traking now.
// https://github.com/microsoft/vscode-cpptools/issues/11164
#ifndef __FILE_NAME__
#define __FILE_NAME__ __FILE__
#endif

#define LOGE(fmt, ...)                                                         \
  __android_log_print(ANDROID_LOG_ERROR, ERR_TAG, "%s:%d %s: " fmt,            \
                      __FILE_NAME__, __LINE__, __PRETTY_FUNCTION__,            \
                      ##__VA_ARGS__)
#define LOGW(fmt, ...)                                                         \
  __android_log_print(ANDROID_LOG_WARN, WARN_TAG, "%s:%d %s: " fmt,            \
                      __FILE_NAME__, __LINE__, __PRETTY_FUNCTION__,            \
                      ##__VA_ARGS__)
#define LOGD(fmt, ...)                                                         \
  __android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "%s:%d %s: " fmt,          \
                      __FILE_NAME__, __LINE__, __PRETTY_FUNCTION__,            \
                      ##__VA_ARGS__)
#define LOGI(fmt, ...)                                                         \
  __android_log_print(ANDROID_LOG_INFO, INFO_TAG, "%s:%d %s: " fmt,            \
                      __FILE_NAME__, __LINE__, __PRETTY_FUNCTION__,            \
                      ##__VA_ARGS__)