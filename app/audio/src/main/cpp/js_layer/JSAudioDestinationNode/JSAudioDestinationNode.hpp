#pragma once
#include "AudioDestinationNode.hpp"
#include "JSAudioNode.hpp"
#include <napi_ih/napi_ih.hpp>

namespace js_audio {
class JSAudioDestinationNode : public JSAudioNode {
public:
  JSAudioDestinationNode(const Napi_IH::IHCallbackInfo &info,
                         std::shared_ptr<AudioDestinationNode>
                             audio_destination_node_ptr = nullptr);

  static void Init(Napi::Env env, Napi::Object exports);

private:
  std::weak_ptr<AudioDestinationNode> audio_destination_node_ref_;
};
} // namespace js_audio