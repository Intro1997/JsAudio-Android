NOTE:

1. `All Support`: support all properties listed in [web audio](https://webaudio.github.io/web-audio-api/)
2. All passed wpt test are list in [wpt_test](../js_tests/wpt_test) folder.

## AudioBuffer

### Feature

All Support

### WPT Test

| Wpt Test                                       | Test State                        | Description                     |
| ---------------------------------------------- | --------------------------------- | ------------------------------- |
| acquire-the-content.html                       | <font color="red">Not Test</font> | Not implement ConvolverNode     |
| audiobuffer-copy-channel.html                  | <font color="green">Pass</font>   |                                 |
| audiobuffer-getChannelData.html                | <font color="green">Pass</font>   |                                 |
| audiobuffer-reuse.html                         | <font color="red">Not Test</font> | Not implement ChannelMergerNode |
| audiobuffer.html                               | <font color="green">Pass</font>   |                                 |
| ctor-audiobuffer.html                          | <font color="green">Pass</font>   |                                 |
| crashtests/copyFromChannel-bufferOffset-1.html | <font color="green">Pass</font>   |                                 |
| crashtests/copyToChannel-bufferOffset-1.html   | <font color="green">Pass</font>   |                                 |

## AudioContext

### Feature

| Feature                                                         | Support State |
| --------------------------------------------------------------- | ------------- |
| constructor                                                     | ✅            |
| baseLatency                                                     | ❌            |
| outputLatency                                                   | ❌            |
| sinkId                                                          | ❌            |
| renderCapacity                                                  | ❌            |
| onsinkchange                                                    | ❌            |
| onerror                                                         | ❌            |
| getOutputTimestamp()                                            | ❌            |
| resume()                                                        | ❌            |
| suspend()                                                       | ❌            |
| close()                                                         | ❌            |
| setSinkId((DOMString or AudioSinkOptions) sinkId)               | ❌            |
| createMediaElementSource(HTMLMediaElement mediaElement)         | ❌            |
| createMediaStreamSource(MediaStream mediaStream)                | ❌            |
| createMediaStreamTrackSource(MediaStreamTrack mediaStreamTrack) | ❌            |
| createMediaStreamDestination()                                  | ❌            |

### WPT Test

Not Test Yet

## AudioDestinationNode

### Feature

All Support

### WPT Test

Pass All Test

## AudioNode

### Feature

| Feature                                     | Support State  |
| ------------------------------------------- | -------------- |
| connect (destinationNode)                   | ✅             |
| connect (destinationNode, output, input)    | ❌             |
| connect (destinationParam, output)          | ❌             |
| disconnect ()                               | ✅             |
| disconnect (output)                         | ❌             |
| disconnect (destinationNode)                | ❌             |
| disconnect (destinationNode, output)        | ❌             |
| disconnect (destinationNode, output, input) | ❌             |
| disconnect (destinationParam)               | ❌             |
| disconnect (destinationParam, output)       | ❌             |
| context                                     | ✅             |
| numberOfInputs                              | ✅             |
| numberOfOutputs                             | ✅             |
| channelCount                                | ✅ (read only) |
| channelCountMode                            | ✅ (read only) |
| channelInterpretation                       | ✅ (read only) |

### WPT Test

Not Test Yet

## AudioParam

### Feature

| Feature                                           | Support State                |
| ------------------------------------------------- | ---------------------------- |
| value                                             | ✅                           |
| automationRate                                    | ✅ (read only) (k-rate only) |
| defaultValue                                      | ✅                           |
| minValue                                          | ✅                           |
| maxValue                                          | ❌                           |
| setValueAtTime (value, startTime)                 | ❌                           |
| linearRampToValueAtTime (value, endTime)          | ❌                           |
| exponentialRampToValueAtTime (value, endTime)     | ❌                           |
| setTargetAtTime (target, startTime, timeConstant) | ❌                           |
| setValueCurveAtTime (values, startTime, duration) | ❌                           |
| cancelScheduledValues (cancelTime)                | ❌                           |
| cancelAndHoldAtTime (cancelTime)                  | ❌                           |

### WPT Test

Not Test Yet

## AudioScheduledSourceNode

### Feature

| Feature     | Support State |
| ----------- | ------------- |
| onended     | ❌            |
| start(when) | ✅            |
| stop(when)  | ✅            |

### WPT Test

Not Test Yet (No Wpt Test)

## BaseAudioContext

### Feature

| Feature                                                                           | Support State |
| --------------------------------------------------------------------------------- | ------------- |
| destination                                                                       | ✅            |
| sampleRate                                                                        | ✅            |
| currentTime                                                                       | ✅            |
| listener                                                                          | ❌            |
| state                                                                             | ❌            |
| renderQuantumSize                                                                 | ❌            |
| audioWorklet                                                                      | ❌            |
| onstatechange                                                                     | ❌            |
| createAnalyser ()                                                                 | ❌            |
| createBiquadFilter ()                                                             | ❌            |
| createBuffer (numberOfChannels, length, sampleRate)                               | ✅            |
| createBufferSource ()                                                             | ✅            |
| createChannelMerger (numberOfInputs)                                              | ❌            |
| createChannelSplitter ( numberOfOutputs)                                          | ❌            |
| createConstantSource ()                                                           | ❌            |
| createConvolver ()                                                                | ❌            |
| createDelay (maxDelayTime)                                                        | ✅            |
| createDynamicsCompressor ()                                                       | ❌            |
| createGain ()                                                                     | ✅            |
| createIIRFilter (feedforward, feedback)                                           | ❌            |
| createOscillator ()                                                               | ✅            |
| createPanner ()                                                                   | ❌            |
| createPeriodicWave (real, imag, constraints)                                      | ❌            |
| createScriptProcessor( bufferSize, numberOfInputChannels, numberOfOutputChannels) | ❌            |
| createStereoPanner ()                                                             | ❌            |
| createWaveShaper ()                                                               | ❌            |
| decodeAudioData ( audioData, successCallback, errorCallback)                      | ❌            |

### WPT Test

Not Test Yet (No Wpt Test)

## OfflineAudioCompletionEvent

### Feature

| Feature                                       | Support State |
| --------------------------------------------- | ------------- |
| constructor (type, eventInitDict)             | ❌            |
| readonly attribute AudioBuffer renderedBuffer | ✅            |

### WPT Test

Not Test Yet (No Wpt Test)

## OfflineAudioContext

### Feature

| Feature                                           | Support State |
| ------------------------------------------------- | ------------- |
| constructor(contextOptions)                       | ✅            |
| constructor(numberOfChannels, length, sampleRate) | ✅            |
| startRendering()                                  | ✅            |
| resume()                                          | ❌            |
| suspend(double suspendTime)                       | ❌            |
| length                                            | ✅            |
| oncomplete                                        | ❌            |

### WPT Test

| Wpt Test                                            | Test State                        | Description                           |
| --------------------------------------------------- | --------------------------------- | ------------------------------------- |
| ctor-offlineaudiocontext.html                       | <font color="green">Pass</font>   |                                       |
| current-time-block-size.html                        | <font color="green">Pass</font>   |                                       |
| offlineaudiocontext-detached-execution-context.html | <font color="red">Not Test</font> | do not support document in NodeJS env |
| startrendering-after-discard.html                   | <font color="red">Not Test</font> | do not support document in NodeJS env |

## OscillatorNode

### Feature

| Feature                                                        | Support State |
| -------------------------------------------------------------- | ------------- |
| constructor (context, optional OscillatorOptions options = {}) | ✅            |
| type                                                           | ✅            |
| frequency                                                      | ✅            |
| detune                                                         | ✅            |
| setPeriodicWave (PeriodicWave periodicWave)                    | ❌            |

### WPT Test

| Wpt Test                          | Test State                               | Description                             |
| --------------------------------- | ---------------------------------------- | --------------------------------------- |
| ctor-oscillator.html              | <font color="yellow">Not Pass All</font> | Not implement PeriodicWave              |
| detune-limiting.html              | <font color="red">Not Test</font>        | Not implement ChannelMergerNode         |
| detune-overflow.html              | <font color="green">Pass</font>          |                                         |
| osc-basic-waveform.html           | <font color="yellow">Not Pass All</font> | Not implement PeriodicWave              |
| crashtests/stop-before-start.html | <font color="red">Not Test</font>        | Not implement AudioParam.setValueAtTime |

## DelayNode

### Feature

All Support

### WPT Test

| Wpt Test                            | Test State                        | Description                                                              |
| ----------------------------------- | --------------------------------- | ------------------------------------------------------------------------ |
| ctor-delay.html                     | <font color="green">Pass</font>   |                                                                          |
| delay-test.html                     | <font color="red">Not Test</font> | Not implement ConstantSourceNode                                         |
| delaynode-channel-count-1.html      | <font color="red">Not Test</font> | Not implement ChannelSplitterNode, ChannelMergerNode, ConstantSourceNode |
| delaynode-max-default-delay.html    | <font color="green">Pass</font>   |                                                                          |
| delaynode-max-nondefault-delay.html | <font color="green">Pass</font>   |                                                                          |
| delaynode-maxdelay.html             | <font color="green">Pass</font>   |                                                                          |
| delaynode-maxdelaylimit.html        | <font color="green">Pass</font>   |                                                                          |
| delaynode-scheduling.html           | <font color="red">Not Test</font> | Not implement AudioParam.setValueAtTime                                  |
| delaynode.html                      | <font color="green">Pass</font>   |                                                                          |
| maxdelay-rounding.html              | <font color="red">Not Test</font> | Not implement ConstantSourceNode                                         |
| no-dezippering.html                 | <font color="red">Not Test</font> | Not implement ConstantSourceNode,AudioParam.setValueAtTime               |

## GainNode

### Feature

All Support

### WPT Test

| Wpt Test            | Test State                        | Description                                         |
| ------------------- | --------------------------------- | --------------------------------------------------- |
| ctor-gain.html      | <font color="green">Pass</font>   |                                                     |
| gain-basic.html     | <font color="green">Pass</font>   |                                                     |
| gain.html           | <font color="green">Pass</font>   |                                                     |
| no-dezippering.html | <font color="red">Not Test</font> | Not implement ConstantSourceNode, ChannelMergerNode |

## AudioBufferSourceNode

### Feature

All Support

### WPT Test

| Wpt Test                                 | Test State                        | Description                                    |
| ---------------------------------------- | --------------------------------- | ---------------------------------------------- |
| active-processing.https.html             | <font color="red">Not Test</font> | Not implement AudioWorkletNode                 |
| audiobuffersource-basic.html             | <font color="green">Pass</font>   |                                                |
| audiobuffersource-channels.html          | <font color="green">Pass</font>   |                                                |
| audiobuffersource-duration-loop.html     | <font color="green">Pass</font>   |                                                |
| audiobuffersource-ended.html             | <font color="red">Not Test</font> | Not implement AudioScheduledSourceNode.onended |
| audiobuffersource-grain.html             | <font color="green">Pass</font>   |                                                |
| audiobuffersource-multi-channels.html    | <font color="red">Not Test</font> | Not implement AudioContext.decodeAudioData     |
| audiobuffersource-null.html              | <font color="green">Pass</font>   |                                                |
| audiobuffersource-one-sample-loop.html   | <font color="green">Pass</font>   |                                                |
| audiobuffersource-playbackrate-zero.html | <font color="red">Not Test</font> | Not implement AudioParam.setValueAtTime        |
| audiobuffersource-start.html             | <font color="green">Pass</font>   |                                                |
| audiosource-onended.html                 | <font color="red">Not Test</font> | Not implement AudioScheduledSourceNode.onended |
| audiosource-time-limits.html             | <font color="green">Pass</font>   |                                                |
| buffer-resampling.html                   | <font color="red">Not Test</font> | Not implement ChannelMergerNode                |
| ctor-audiobuffersource.html              | <font color="green">Pass</font>   |                                                |
| note-grain-on-play.html                  | <font color="green">Pass</font>   |                                                |
| note-grain-on-timing.html                | <font color="green">Pass</font>   |                                                |
| sample-accurate-scheduling.html          | <font color="green">Pass</font>   |                                                |
| sub-sample-buffer-stitching.html         | <font color="red">Not Test</font> | Not implement ChannelMergerNode                |
| sub-sample-scheduling.html               | <font color="red">Not Test</font> | Not implement ChannelMergerNode                |
