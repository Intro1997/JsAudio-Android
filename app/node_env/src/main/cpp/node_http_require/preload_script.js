let loadFileContentFromJSEntry = null;
const Module = require("module");

const originFindPath = Module._findPath;
Module._findPath = function (request, paths, isMain) {
  // TODO: use array to save folder that need to be loaded by http
  if (request.includes("js_tests") || request.includes("./")) {
    return request;
  } else {
    originFindPath(request, paths, isMain);
  }
};

Module._extensions[".js"] = function (module, filename) {
  if (!loadFileContentFromJSEntry) {
    loadFileContentFromJSEntry =
      process._linkedBinding("node_http_require").loadFileContentFromJsEntry;
  }

  if (!loadFileContentFromJSEntry) {
    console.error("Get loadFileContentFromJSEntry function failed!");
    return;
  }

  const file_script = loadFileContentFromJSEntry(filename);
  if (!file_script || file_script === "") {
    console.error(`Find ${filename} in http failed!`);
    return;
  }

  module._compile(file_script, filename);
};

globalThis.require = Module.createRequire(process.cwd() + "/");
