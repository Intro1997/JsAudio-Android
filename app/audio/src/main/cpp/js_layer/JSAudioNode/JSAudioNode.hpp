#pragma once
#include "AudioNode.hpp"
#include <napi_ih/napi_ih.hpp>
namespace js_audio {
class JSAudioNode : public Napi_IH::IHObjectWrap {
public:
  JSAudioNode(const Napi_IH::IHCallbackInfo &info,
              std::shared_ptr<AudioNode> audio_destination_node_ref = nullptr);
  static void Init(Napi::Env env, Napi::Object exports);

  static bool ExtractOptionsFromInfo(const std::string &audio_node_type_name,
                                     const Napi_IH::IHCallbackInfo &info,
                                     AudioNode::AudioNodeOptions &options,
                                     Napi::Error &napi_error);

protected:
  std::shared_ptr<AudioNode> audio_node_ref_;

private:
  Napi::Value getContext(const Napi::CallbackInfo &info);
  Napi::Value getNumberOfInputs(const Napi::CallbackInfo &info);
  Napi::Value getNumberOfOutputs(const Napi::CallbackInfo &info);
  Napi::Value getChannelCount(const Napi::CallbackInfo &info);
  Napi::Value getChannelCountMode(const Napi::CallbackInfo &info);
  Napi::Value getChannelInterpretation(const Napi::CallbackInfo &info);

  Napi::Value connect(const Napi::CallbackInfo &info);
  Napi::Value disconnect(const Napi::CallbackInfo &info);

  // weak ref
  Napi::ObjectReference napi_audio_context_ptr_;
};
} // namespace js_audio