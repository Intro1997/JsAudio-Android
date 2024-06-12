const fs = require("fs");
const cpp_module_paths = ["app/audio", "app/node_env"];
const compile_mode = "debug";
const arch = "arm64-v8a";
const output_file_path = ".vscode/compile_commands.json";

CombineModuleCompileCommands(cpp_module_paths, output_file_path);

function CombineModuleCompileCommands(cpp_module_paths, output_file_path) {
  const cpp_module_compile_commands_paths = GetCommandPath(cpp_module_paths);
  console.log("Try to combine following compile config files:");
  console.log(cpp_module_compile_commands_paths, "\n");
  CombineCompileCommands(cpp_module_compile_commands_paths, output_file_path);
}

function CombineCompileCommands(
  cpp_module_compile_commands_paths,
  output_file_path
) {
  const config_elements = [];
  const success_config = [];
  cpp_module_compile_commands_paths.forEach((cc_path) => {
    if (!fs.existsSync(cc_path)) {
      console.error(`Cannot find path ${cc_path}`);
    } else {
      let file_content = "";
      try {
        file_content = fs.readFileSync(cc_path, "utf-8");
        file_content_array = JSON.parse(file_content);
        file_content_array.forEach((element) => {
          config_elements.push(element);
        });
        success_config.push(cc_path);
      } catch (e) {
        console.error("Get file content failed!");
        console.log(`file_content = ${file_content}`);
      }
    }
  });
  const combine_ret = JSON.stringify(config_elements);
  fs.writeFileSync(output_file_path, combine_ret);
  console.log("Success combine following compile config files:");
  console.log(success_config);
}

function GetCommandPath(cpp_module_paths) {
  const compile_commands_file_path = [];
  cpp_module_paths.forEach((module_path) => {
    module_path = FormatPath(module_path);
    if (!fs.existsSync(module_path)) {
      console.error(`cannot find ${module_path}`);
    } else {
      compile_commands_file_path.push(
        module_path +
          `/.cxx/tools/${compile_mode}/${arch}/compile_commands.json`
      );
    }
  });
  return compile_commands_file_path;
}

function FormatPath(path_str) {
  let ret = path_str;
  if (path_str[path_str.length - 1] == "/") {
    ret = path_str.slice(0, path_str.length - 1);
  }
  return ret;
}
