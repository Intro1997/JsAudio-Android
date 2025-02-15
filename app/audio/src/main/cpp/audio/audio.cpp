
#include <jni.h>
#include <napi/napi.h>

#include <string>

#include "AudioEngine.hpp"
#include "JSAudioBuffer.hpp"
#include "JSAudioBufferSourceNode.hpp"
#include "JSAudioContext.hpp"
#include "JSAudioDestinationNode.hpp"
#include "JSAudioNode.hpp"
#include "JSAudioParam.hpp"
#include "JSAudioScheduledSourceNode.hpp"
#include "JSBaseAudioContext.hpp"
#include "JSDelayNode.hpp"
#include "JSGainNode.hpp"
#include "JSOfflineAudioCompletionEvent.hpp"
#include "JSOfflineAudioContext.hpp"
#include "JSOscillatorNode.hpp"
#include "preload_script.hpp"

static Napi::Object Init(Napi::Env env, Napi::Object exports) {
  js_audio::JSOfflineAudioCompletionEvent::Init(env, exports);
  js_audio::JSAudioBuffer::Init(env, exports);
  js_audio::JSAudioParam::Init(env, exports);
  js_audio::JSAudioNode::Init(env, exports);
  js_audio::JSAudioDestinationNode::Init(env, exports);
  js_audio::JSAudioScheduledSourceNode::Init(env, exports);
  js_audio::JSOscillatorNode::Init(env, exports);
  js_audio::JSBaseAudioContext::Init(env, exports);
  js_audio::JSAudioContext::Init(env, exports);
  js_audio::JSOfflineAudioContext::Init(env, exports);
  js_audio::JSGainNode::Init(env, exports);
  js_audio::JSDelayNode::Init(env, exports);
  js_audio::JSAudioBufferSourceNode::Init(env, exports);

  return exports;
}

NAPI_IH_API_MODULE(audio, Init);