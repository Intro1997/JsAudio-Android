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

## clang-tidy

If you find pre-commit in husky run when execute `git commit` will occurs some errors about system header, but these errors are disappear when execute `npx lint-staged` in terminal, just ignore them with `--no-verify` such as `git commit -m "xxx" --no-verify`.

Or if you fix that, please tell me how to do it by create `Issue`, Thanks!

## NAPI

### NodeJS

The `libnode.so ` built with [nodejs-mobile](https://github.com/nodejs-mobile/nodejs-mobile) on macOS in [lima](https://github.com/lima-vm/lima) and [containerd](https://github.com/containerd/containerd) x86_64 linux vm. More detail please read [build_nodejs.md](./build_nodejs.md).

### NAPI_IH

The goal of the project architecture is to make the structure of the napi layer consistent with the cpp layer. But NAPI do not support inheritance, so we make a patch to NAPI, that is [NAPI_IH](./napi_ih_api.md).

## Issues

If you meet crash, please follow the steps below to clean all cache of cpp and android:

1. delete `app/build` folder
2. delete `app/audio/.cxx` and `app/audio/build` folder
3. delete `app/node_env/.cxx` and `app/node_env/build` folder
4. open Android Studio, select Build -> Clean Project
5. open Android Studio, select File -> Invalidate Cache, check all options that you can check, then select Invalidate and Restart.
6. Rebuild project and run

If crash still exists, please create issues with your android studio log.

It's also a good idea to debug with debug version of NodeJS so.
