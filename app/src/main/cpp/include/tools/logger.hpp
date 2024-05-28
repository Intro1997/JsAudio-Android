//
// Created by Outro on 2023/10/11.
//

#ifndef WALLPAPER3D_APP_SRC_MAIN_CPP_COMMON_LOGGER_HPP_
#define WALLPAPER3D_APP_SRC_MAIN_CPP_COMMON_LOGGER_HPP_
#include <android/log.h>
#define LOG_TAG "Native Message"

// avoid c/c++ extension bug, still traking now.
// https://github.com/microsoft/vscode-cpptools/issues/11164
#ifndef __FILE_NAME__
#define __FILE_NAME__ __FILE__
#endif

#define LOGE(fmt, ...)                                                         \
  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%s:%d %s: " fmt,            \
                      __FILE_NAME__, __LINE__, __PRETTY_FUNCTION__,            \
                      ##__VA_ARGS__)
#define LOGW(fmt, ...)                                                         \
  __android_log_print(ANDROID_LOG_WARN, LOG_TAG, "%s:%d %s: " fmt,             \
                      __FILE_NAME__, __LINE__, __PRETTY_FUNCTION__,            \
                      ##__VA_ARGS__)
#define LOGD(fmt, ...)                                                         \
  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "%s:%d %s: " fmt,            \
                      __FILE_NAME__, __LINE__, __PRETTY_FUNCTION__,            \
                      ##__VA_ARGS__)
#define LOGI(fmt, ...)                                                         \
  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "%s:%d %s: " fmt,             \
                      __FILE_NAME__, __LINE__, __PRETTY_FUNCTION__,            \
                      ##__VA_ARGS__)
#endif // WALLPAPER3D_APP_SRC_MAIN_CPP_COMMON_LOGGER_HPP_