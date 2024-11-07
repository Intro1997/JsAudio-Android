# Run

1. run `npm install` for installing dependency
2. run `npm start` to get server address like `http://xxx.xxx.xxx.xxx:20490`
3. add this line `js_entry="http://xxx.xxx.xxx.xxx:20490/index.js"` to local.property file

# Develop

## VSCode

1. You should open root folder by vscode after running app or building app at `app/build.gradle.kts` page. This will make Android Studio generate `.vscode/compile_commands.json` file, which is used by vscode to recognize cpp headers.

# Note:

1. `combine` script in package.json is used to make vscode recognize all cpp files in different module inside android module. When Android Build success, scripts in `build.gradle.kts` will move all `compile_commands.json` contents to `.vscode/compile_commands.json`, this will make vscode cpp intelligence works fine.

# Plan

| Features                    | Complete State                                             | Test State                                                                                               |
| --------------------------- | ---------------------------------------------------------- | -------------------------------------------------------------------------------------------------------- |
| AudioBuffer                 | <font color="green">Complete</font>                        | <font color="yellow">Not Pass All Wpt Test (lack support of "ConvolverNode", "ChannelMergerNode")</font> |
| AudioContext                | <font color="green">Complete</font>                        | <font color="yellow">Not Test Yet</font>                                                                 |
| AudioDestinationNode        | <font color="yellow">1. All data member is readonly</font> | <font color="green">Complete</font>                                                                      |
| AudioNode                   | <font color="green">Complete</font>                        | <font color="red">Not Test Yet</font>                                                                    |
| AudioParam                  | <font color="green">Complete</font>                        | <font color="red">Not Test Yet</font>                                                                    |
| AudioScheduledSourceNode    | <font color="green">Complete</font>                        | <font color="grey">No Wpt Test</font>                                                                    |
| BaseAudioContext            | <font color="green">Complete</font>                        | <font color="grey">No Wpt Test</font>                                                                    |
| OfflineAudioCompletionEvent | <font color="green">Complete</font>                        | <font color="grey">No Wpt Test</font>                                                                    |
| OfflineAudioContext         | <font color="green">Complete</font>                        | <font color="green">Complete</font>                                                                      |
| OscillatorNode              | <font color="green">Complete</font>                        | <font color="yellow">Not Pass All Wpt Test (lack support of "ChannelMergerNode")</font>                  |
| GainNode                    | <font color="green">Complete</font>                        | <font color="yellow">Not Pass All Wpt Test (lack support of "ChannelMergerNode")</font>                  |
| DelayNode                   | <font color="green">Complete</font>                        | <font color="red">Not Test Yet (lack support of "ConstantSourceNode", "AudioBufferSourceNode", "ChannelSplitterNode", "ChannelMergerNode")</font>    |
| AudioBufferSourceNode       | <font color="red">Not Complete</font>                      | <font color="red">Not Test Yet (lack support of "AudioWorkletNode", "ChannelMergerNode")</font>                                                                    |
| MediaRecorderNode           | <font color="red">Not Complete</font>                      | <font color="red">Not Test Yet</font>                                                                    |
| MicrophoneStreamNode        | <font color="red">Not Complete</font>                      | <font color="red">Not Test Yet</font>                                                                    |
