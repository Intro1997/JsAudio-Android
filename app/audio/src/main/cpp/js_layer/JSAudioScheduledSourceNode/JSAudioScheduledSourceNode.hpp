#pragma once
#include "AudioScheduledSourceNode.hpp"
#include "JSAudioNode.hpp"
namespace js_audio {
class JSAudioScheduledSourceNode : public JSAudioNode {
public:
  JSAudioScheduledSourceNode(const Napi_IH::IHCallbackInfo &info,
                             std::shared_ptr<AudioScheduledSourceNode>
                                 audio_scheduled_source_node_ref = nullptr);

  static void Init(Napi::Env env, Napi::Object exports);

  virtual Napi::Value start(const Napi::CallbackInfo &info);
  Napi::Value stop(const Napi::CallbackInfo &info);

protected:
  bool IsWhenValid(const double &when, const char *type_name,
                   const char *func_name, Napi::Env env,
                   Napi::Error &napi_error);
};
} // namespace js_audio
