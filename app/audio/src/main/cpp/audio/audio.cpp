
#include <jni.h>
#include <napi/napi.h>
#include <string>

#include "AudioBufferQueuePlayer.hpp"
#include "AudioEngine.hpp"
#include "audio.hpp"
#include "logger.hpp"
#include "preload_script.hpp"

#include "JSAudioContext.hpp"
#include "JSAudioDestinationNode.hpp"
#include "JSAudioNode.hpp"

static Napi::Object Init(Napi::Env env, Napi::Object exports) {
  js_audio::JSAudioNode::Init(env, exports);
  js_audio::JSAudioDestinationNode::Init(env, exports);
  js_audio::JSAudioContext::Init(env, exports);
  return exports;
}

NODE_API_MODULE(audio, Init);