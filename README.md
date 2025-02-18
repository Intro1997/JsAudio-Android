![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![NodeJS v18.20.4](https://img.shields.io/badge/NodeJS-v18.20.4-blue.svg)

# Description

This project aims to run Web Audio API in Android with javascript code. Most of the available APIs have passed wpt test cases.

Now it supports following Web Audio feature:

| Features                    | Interface Support State                                              | Test State                                                                 |
| --------------------------- | -------------------------------------------------------------------- | -------------------------------------------------------------------------- |
| AudioBuffer                 | ![Static Badge](https://img.shields.io/badge/Full_Support-green)     | ![Static Badge](https://img.shields.io/badge/Not_Pass_All_Wpt_Test-orange) |
| AudioContext                | ![Static Badge](https://img.shields.io/badge/Partial_Support-orange) | ![Static Badge](https://img.shields.io/badge/Not_Test_Yet-%23FF0000)       |
| AudioDestinationNode        | ![Static Badge](https://img.shields.io/badge/Full_Support-green)     | ![Static Badge](https://img.shields.io/badge/Complete-green)               |
| AudioNode                   | ![Static Badge](https://img.shields.io/badge/Partial_Support-orange) | ![Static Badge](https://img.shields.io/badge/Not_Test_Yet-%23FF0000)       |
| AudioParam                  | ![Static Badge](https://img.shields.io/badge/Partial_Support-orange) | ![Static Badge](https://img.shields.io/badge/Not_Test_Yet-%23FF0000)       |
| AudioScheduledSourceNode    | ![Static Badge](https://img.shields.io/badge/Partial_Support-orange) | ![Static Badge](https://img.shields.io/badge/No_Wpt_Test-grey)             |
| BaseAudioContext            | ![Static Badge](https://img.shields.io/badge/Partial_Support-orange) | ![Static Badge](https://img.shields.io/badge/No_Wpt_Test-grey)             |
| OfflineAudioCompletionEvent | ![Static Badge](https://img.shields.io/badge/Partial_Support-orange) | ![Static Badge](https://img.shields.io/badge/No_Wpt_Test-grey)             |
| OfflineAudioContext         | ![Static Badge](https://img.shields.io/badge/Partial_Support-orange) | ![Static Badge](https://img.shields.io/badge/Complete-green)               |
| OscillatorNode              | ![Static Badge](https://img.shields.io/badge/Partial_Support-orange) | ![Static Badge](https://img.shields.io/badge/Not_Pass_All_Wpt_Test-orange) |
| GainNode                    | ![Static Badge](https://img.shields.io/badge/Full_Support-green)     | ![Static Badge](https://img.shields.io/badge/Not_Pass_All_Wpt_Test-orange) |
| DelayNode                   | ![Static Badge](https://img.shields.io/badge/Full_Support-green)     | ![Static Badge](https://img.shields.io/badge/Not_Pass_All_Wpt_Test-orange) |
| AudioBufferSourceNode       | ![Static Badge](https://img.shields.io/badge/Full_Support-green)     | ![Static Badge](https://img.shields.io/badge/Not_Pass_All_Wpt_Test-orange) |

more detail info is descript [here](./js_api_define/features.md).

# Build & Run

1. Download `NodeJS v18.20.4` library from [Release](https://github.com/Intro1997/JsAudio-Android/releases)，put so in `./app/dependency/cpp/node/library` and named `libnode.so`
2. run `npm install` for installing dependency
3. run `npm start` to get server address like `http://xxx.xxx.xxx.xxx:20490`
4. open root folder with Android Studio
5. add this line `js_entry="http://xxx.xxx.xxx.xxx:20490/index.js"` to local.property file
6. build and run app

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

1. The NodeJS library is built base on [nodejs-mobile](https://github.com/nodejs-mobile/nodejs-mobile) v18.20.4 release verison by [lima](https://github.com/lima-vm/lima) and [containerd](https://github.com/containerd/containerd), Thanks!
2. Test case based on [wpt](https://github.com/web-platform-tests/wpt).
3. NAPI_IH base on [node-addon-api-helper](https://github.com/ajihyf/node-addon-api-helper).
4. Use [ProcessPhoenix](https://github.com/JakeWharton/ProcessPhoenix) to restart Android App.
