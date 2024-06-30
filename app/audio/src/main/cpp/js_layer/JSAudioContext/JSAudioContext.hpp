#pragma once
#include <napi/napi.h>
namespace js_audio {
class JSAudioContext : public Napi::ObjectWrap<JSAudioContext> {
public:
  static Napi::Value Init(Napi::Env napi_env, Napi::Object exports);

public:
  JSAudioContext(const Napi::CallbackInfo &info);

private:
  Napi::Value GetSampleRate(const Napi::CallbackInfo &info);
  Napi::Value GetDestinationNode(const Napi::CallbackInfo &info);
  Napi::ObjectReference js_destination_node_ref_;
  double sample_rate_;
};
} // namespace js_audio
