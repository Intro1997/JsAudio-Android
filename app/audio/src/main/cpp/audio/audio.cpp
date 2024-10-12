
#include <jni.h>
#include <napi/napi.h>
#include <string>


#include "AudioEngine.hpp"
#include "audio.hpp"
#include "logger.hpp"
#include "preload_script.hpp"

#include "JSAudioBuffer.hpp"
#include "JSAudioContext.hpp"
#include "JSAudioDestinationNode.hpp"
#include "JSAudioNode.hpp"
#include "JSAudioParam.hpp"
#include "JSAudioScheduledSourceNode.hpp"
#include "JSBaseAudioContext.hpp"
#include "JSOscillatorNode.hpp"

static Napi::Object Init(Napi::Env env, Napi::Object exports) {
  js_audio::JSAudioBuffer::Init(env, exports);
  js_audio::JSAudioParam::Init(env, exports);
  js_audio::JSAudioNode::Init(env, exports);
  js_audio::JSAudioDestinationNode::Init(env, exports);
  js_audio::JSAudioScheduledSourceNode::Init(env, exports);
  js_audio::JSOscillatorNode::Init(env, exports);
  js_audio::JSBaseAudioContext::Init(env, exports);
  js_audio::JSAudioContext::Init(env, exports);
  
  return exports;
}

NAPI_IH_API_MODULE(audio, Init);