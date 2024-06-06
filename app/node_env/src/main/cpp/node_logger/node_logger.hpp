#pragma once
#include <node/node.h>
namespace node_logger {
void Init(v8::Local<v8::Object> exports, v8::Local<v8::Value> module,
          v8::Local<v8::Context> context, void *priv);

const char *GetPreLoadScript();

} // namespace node_logger
