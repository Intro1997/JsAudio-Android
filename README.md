# Run

1. run `npm install` for installing dependency
2. run `npm start` to get server address like `http://xxx.xxx.xxx.xxx:20490`
3. add this line `js_entry="http://xxx.xxx.xxx.xxx:20490/index.js"` to local.property file

# Develop

## VSCode

1. You should open root folder by vscode after running app or building app at `app/build.gradle.kts` page. This will make Android Studio generate `.vscode/compile_commands.json` file, which is used by vscode to recognize cpp headers.

# Note:

1. `combine` script in package.json is used to make vscode recognize all cpp files in different module inside android module. When Android Build success, scripts in `build.gradle.kts` will move all `compile_commands.json` contents to `.vscode/compile_commands.json`, this will make vscode cpp intelligence works fine.
