#include "node_logger.hpp"
#include "logger.hpp"
#include "preload_script.hpp"
#include <node/node.h>

namespace node_logger {

enum class LogType { kLog = 0, kInfo, kWarn, kError };

// to avoid outputed message of logcat be cut off
const static size_t kMaxOutputSize = 1023;

static void InnerLog(const std::string &message, const LogType type) {
  android_LogPriority output_type;
  size_t message_size = message.size();

  switch (type) {
  case LogType::kLog: {
    output_type = ANDROID_LOG_DEBUG;
    LOGD("[JS LOG]: ");
    break;
  }
  case LogType::kInfo: {
    output_type = ANDROID_LOG_INFO;
    LOGI("[JS LOG]: ");
    break;
  }
  case LogType::kWarn: {
    output_type = ANDROID_LOG_WARN;
    LOGW("[JS LOG]: ");
    break;
  }
  case LogType::kError: {
    output_type = ANDROID_LOG_ERROR;
    LOGE("[JS LOG]: ");
    break;
  }
  default:
    break;
  }
  size_t current_start = 0;
  size_t current_end = kMaxOutputSize;
  do {
    if (current_end > message_size) {
      current_end = message_size;
    }
    __android_log_print(output_type, "", "%s",
                        message.substr(current_start, current_end).c_str());
    current_start = current_end;
    current_end += kMaxOutputSize;
  } while (current_start < message_size);
  __android_log_print(output_type, "", "\n");
}

static void Log(const v8::FunctionCallbackInfo<v8::Value> &info) {
  if (info.Length() > 0 && info[0]->IsString()) {
    v8::Isolate *isolate = info.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::String::Utf8Value str_value(isolate, info[0]);
    std::string js_message = *str_value;
    uint32_t log_type = 0;
    if (info.Length() > 1 && info[1]->IsNumber()) {
      log_type = (info[1]->ToUint32(context).ToLocalChecked())->Value();
    }
    InnerLog(js_message, (LogType)log_type);
  }
}

void Init(v8::Local<v8::Object> exports, v8::Local<v8::Value> module,
          v8::Local<v8::Context> context, void *priv) {
  NODE_SET_METHOD(exports, "log", Log);
}

const char *GetPreloadScript() { return NODE_LOGGER_PRELOAD_SCRIPT; }

} // namespace node_logger