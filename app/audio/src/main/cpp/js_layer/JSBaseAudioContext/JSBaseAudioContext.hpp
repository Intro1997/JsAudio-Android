#pragma once
#include "BaseAudioContext.hpp"
#include "JSAudioDestinationNode.hpp"
#include <napi_ih/napi_ih.hpp>
namespace js_audio {
class JSBaseAudioContext : public Napi_IH::IHObjectWrap {
public:
  JSBaseAudioContext(
      const Napi_IH::IHCallbackInfo &info,
      std::shared_ptr<BaseAudioContext> base_audio_context_ref = nullptr);

  static void Init(Napi::Env env, Napi::Object exports);
  std::shared_ptr<std::mutex> GetAudioContextLock() const;
  std::shared_ptr<BaseAudioContext> GetAudioContext() const;
  float GetSampleRate() const;

  Napi::Value getDestinationNode(const Napi::CallbackInfo &info);
  Napi::Value getSampleRate(const Napi::CallbackInfo &info);
  Napi::Value getCurrentTime(const Napi::CallbackInfo &info);
  Napi::Value createOscillator(const Napi::CallbackInfo &info);
  Napi::Value createBuffer(const Napi::CallbackInfo &info);
  Napi::Value createGain(const Napi::CallbackInfo &info);
  Napi::Value createDelay(const Napi::CallbackInfo &info);

protected:
  std::shared_ptr<BaseAudioContext> base_audio_context_ref_;

private:
  Napi::ObjectReference napi_destination_node_ref_;
};
} // namespace js_audio