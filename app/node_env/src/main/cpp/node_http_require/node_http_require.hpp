#pragma once
#include "preload_script.hpp"
#include <node/node.h>
#include <string>
namespace node_http_require {
void Init(v8::Local<v8::Object> exports, v8::Local<v8::Value> module,
          v8::Local<v8::Context> context, void *priv);

const char *GetPreloadScript();
} // namespace node_http_require