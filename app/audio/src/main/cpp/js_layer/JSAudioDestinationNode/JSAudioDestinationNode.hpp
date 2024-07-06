#pragma once
#include "JSAudioNode.hpp"
#include <napi_ih/napi_ih.hpp>

namespace js_audio {
class JSAudioDestinationNode : public JSAudioNode {
public:
  JSAudioDestinationNode(const Napi_IH::IHCallbackInfo &info);

  static void Init(Napi::Env env, Napi::Object exports);

  Napi::Value GetDestinationSpecified(const Napi::CallbackInfo &info);
  void SetDestinationSpecified(const Napi::CallbackInfo &info,
                               const Napi::Value &value);

private:
  uint32_t destination_specified_;
};
} // namespace js_audio