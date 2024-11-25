![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![NodeJS v14.17.0](https://img.shields.io/badge/NodeJS-v14.17.0-blue.svg)

# Description

This project aims to run Web Audio API in Android with javascript code. Most of the available APIs have passed wpt test cases.

Now it supports following Web Audio feature:

| Features                    | Interface Support State                     | Test State                                                                                                                                                        |
| --------------------------- | ------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| AudioBuffer                 | <font color="green">Full Support</font>     | <font color="yellow">Not Pass All Wpt Test (lack support of "ConvolverNode", "ChannelMergerNode")</font>                                                          |
| AudioContext                | <font color="green">Partial Support</font>  | <font color="yellow">Not Test Yet</font>                                                                                                                          |
| AudioDestinationNode        | <font color="yellow">Full Support</font>    | <font color="green">Complete</font>                                                                                                                               |
| AudioNode                   | <font color="yellow">Partial Support</font> | <font color="red">Not Test Yet</font>                                                                                                                             |
| AudioParam                  | <font color="yellow">Partial Support</font> | <font color="red">Not Test Yet</font>                                                                                                                             |
| AudioScheduledSourceNode    | <font color="yellow">Partial Support</font> | <font color="grey">No Wpt Test</font>                                                                                                                             |
| BaseAudioContext            | <font color="yellow">Partial Support</font> | <font color="grey">No Wpt Test</font>                                                                                                                             |
| OfflineAudioCompletionEvent | <font color="yellow">Partial Support</font> | <font color="grey">No Wpt Test</font>                                                                                                                             |
| OfflineAudioContext         | <font color="yellow">Partial Support</font> | <font color="green">Complete</font>                                                                                                                               |
| OscillatorNode              | <font color="yellow">Partial Support</font> | <font color="yellow">Not Pass All Wpt Test (lack support of "ChannelMergerNode")</font>                                                                           |
| GainNode                    | <font color="green">Full Support</font>     | <font color="yellow">Not Pass All Wpt Test (lack support of "ChannelMergerNode")</font>                                                                           |
| DelayNode                   | <font color="green">Full Support</font>     | <font color="yellow">Not Pass All Wpt Test (lack support of "ConstantSourceNode", "ChannelSplitterNode", "ChannelMergerNode")</font>                              |
| AudioBufferSourceNode       | <font color="green">Full Support</font>     | <font color="yellow">Not Pass All Wpt Test (lack support of "AudioWorkletNode", "ChannelMergerNode", "EventListener", "decodeAudioData", "setValueAtTime")</font> |

more detail info is descript [here](./js_api_define/features.md).

# Build & Run

1. run `npm install` for installing dependency
2. run `npm start` to get server address like `http://xxx.xxx.xxx.xxx:20490`
3. open root folder with Android Studio
4. add this line `js_entry="http://xxx.xxx.xxx.xxx:20490/index.js"` to local.property file
5. build and run app

# App Interface

After running app, you can see five buttons:

1. `START`: Start running js code in `index.js`
2. `STOP`: Stop js event loop. You need to press `RESTART` button to re-run js code.
3. `PAUSE`: Pause js event loop.
4. `RESUME`: Resume js event from pause.
5. `RESTART`: Restart app.

NOTE: all console message in js code will be output to Android Studio Logcat.

# Develop

see [develop.md](./docs/develop.md)

# Thanks

1. Test case based on [wpt](https://github.com/web-platform-tests/wpt).
2. NAPI_IH base on [node-addon-api-helper](https://github.com/ajihyf/node-addon-api-helper).
3. Use [ProcessPhoenix](https://github.com/JakeWharton/ProcessPhoenix) to restart Android App.
