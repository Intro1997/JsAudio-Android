#include "node_logger.hpp"
#include "logger.hpp"
#include <node/node.h>

namespace node_logger {
static void logger(const v8::FunctionCallbackInfo<v8::Value> &info) {
  // TODO: only support log pure string here
  if (info.Length() > 0 && info[0]->IsString()) {
    v8::Isolate *isolate = info.GetIsolate();
    v8::String::Utf8Value str_value(isolate, info[0]);
    LOGD("JS-LOG: %s\n", *str_value);
  }
}

void init(v8::Local<v8::Object> exports, v8::Local<v8::Value> module,
          v8::Local<v8::Context> context, void *priv) {
  NODE_SET_METHOD(exports, "node_logger", logger);
}

} // namespace node_logger