#pragma once
#include "BaseAudioContext.hpp"
#include "JSAudioDestinationNode.hpp"
#include <napi_ih/napi_ih.hpp>
namespace js_audio {
class JSBaseAudioContext : public Napi_IH::IHObjectWrap {
public:
  JSBaseAudioContext(
      const Napi_IH::IHCallbackInfo &info,
      std::shared_ptr<BaseAudioContext> base_audio_context_ptr = nullptr);

  static void Init(Napi::Env env, Napi::Object exports);

  Napi::Value GetDestinationNode(const Napi::CallbackInfo &info);
  Napi::Value GetSampleRate(const Napi::CallbackInfo &info);
  Napi::Value GetCurrentTime(const Napi::CallbackInfo &info);

  Napi::Value createBuffer(const Napi::CallbackInfo &info);
private:
  Napi::ObjectReference js_destination_node_ref_;
  float sample_rate_;   // TDOO: move to cpp side
  double current_time_; // TDOO: move to cpp side
  std::shared_ptr<BaseAudioContext> base_audio_context_ptr_;
};
} // namespace js_audio