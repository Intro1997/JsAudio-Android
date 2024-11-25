# Develop

## package.json

We put some useful script in `package.json` file:

```json
"scripts": {
    "compress": "node js_task/compress.js",
    "combine": "node js_task/combine_compile_commands.js",
    "valid": "ROOT_DIR=$(pwd) node ./js_task/valid_task/valid.js",
    "start": "npm run compress && http-server -p 20490"
  },
```

1. `compress`: each NAPI module which needs to be exported in js is export by `globalThis.ModuleApiName = process._linkedBinding("ModuleName").ApiName;`. These js code are put in `**/preload_script.js` file, which will be converted to c string saved in `**/preload_script.hpp`. This process is handled by `compress` command.
2. `combine`: cpp code of this project is developed in vscode, so we need to add all relative file paths to the `compileCommands` options of `.vscode/c_cpp_properties.json`. These cpp relative files will be created when do build in Android Studio, `combine` command is used to collect and add them to `.vscode/compile_commands.json`, then vscode can recognize cpp symbol.
3. `valid`: Use to check if WaveProducer, a tool used in OscillatorNode, works correctly.
4. `start`: do `compress` and start a http-server accessed by app to load js code which need to be run.

## cpp hint

If you write cpp layer in vscode, you should build once after Android Studio sync. Build will trigger `combine` command which makes vscode recognize cpp symbol.

NOTE: if you find vscode not recognize cpp symbol, make sure to switch the Android Studio tab to `app/build.gradle.kts`, then build.

## NAPI

### NodeJS

The node env of this project is based on [v14.17.0](https://github.com/toyobayashi/node-android-build/releases/tag/v14.17.0) of [node-android-build](https://github.com/toyobayashi/node-android-build). The build tutorial is referenced to [在 Android 应用中嵌入 Node.js](https://toyobayashi.github.io/2021/03/29/NodeEmbedding/)

### NAPI_IH

The goal of the project architecture is to make the structure of the napi layer consistent with the cpp layer. But NAPI do not support inheritance, so we make a patch to NAPI, that is [NAPI_IH](./napi_ih_api.md).
