#pragma once
#include "JSAudioNode.hpp"
#include <napi/napi.h>

namespace js_audio {
class JSAudioDestinationNode : public JSAudioNode {
public:
  static Napi::Function GetClass(Napi::Env env);

public:
  JSAudioDestinationNode(const Napi::CallbackInfo &info);
  static void Init(Napi::Env env, Napi::Object exports);

private:
  static Napi::FunctionReference js_destination_node_class_ref_;
};
} // namespace js_audio