#pragma once
#include <node/node.h>
namespace node_logger {
void init(v8::Local<v8::Object> exports, v8::Local<v8::Value> module,
          v8::Local<v8::Context> context, void *priv);

} // namespace node_logger
