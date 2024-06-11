#include "node_logger.hpp"
#include "logger.hpp"
#include "preload_script.hpp"
#include <node/node.h>

namespace node_logger {

enum class LogType { kLog = 0, kInfo, kWarn, kError };

static void InnerLog(const std::string &message, const LogType type) {
  switch (type) {
  case LogType::kLog: {
    LOGD("[JS LOG]: %s\n", message.c_str());
    break;
  }
  case LogType::kInfo: {
    LOGI("[JS LOG]: %s\n", message.c_str());
    break;
  }
  case LogType::kWarn: {
    LOGW("[JS LOG]: %s\n", message.c_str());
    break;
  }
  case LogType::kError: {
    LOGE("[JS LOG]: %s\n", message.c_str());
    break;
  }
  default:
    break;
  }
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