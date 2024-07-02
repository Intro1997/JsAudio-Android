#pragma once
#include "JSAudioNode.hpp"
// #include <napi/napi.h>
#include <napi_ih/napi_ih.hpp>

namespace js_audio {
class JSAudioDestinationNode : public Napi_IH::IHObjectWrap<JSAudioDestinationNode> {
public:
  static Napi::Function GetClass(Napi::Env env);

public:
  JSAudioDestinationNode(const Napi::CallbackInfo &info);
  static void Init(Napi::Env env, Napi::Object exports);
  Napi::Value GetDestinationSpecified(const Napi::CallbackInfo &info);

private:
  static Napi::FunctionReference js_destination_node_class_ref_;
  uint32_t destination_specified_;
};
} // namespace js_audio