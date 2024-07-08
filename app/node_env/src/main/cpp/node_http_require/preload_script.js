let loadFileContentFromJSEntry = null;
const requireFileFromProcess = require("module").createRequire(
  process.cwd() + "/"
);

const requireFileFromJsEntry = (filename) => {
  if (!loadFileContentFromJSEntry) {
    loadFileContentFromJSEntry =
      process._linkedBinding("node_http_require").loadFileContentFromJsEntry;
  }

  const file_script = loadFileContentFromJSEntry(filename);

  if (!file_script || file_script === "") {
    console.error(`Find ${filename} in http failed!`);
    return null;
  }

  const vm = requireFileFromProcess("vm");
  const sandbox = { module: { exports: {} }, console: console, ...globalThis };
  vm.runInNewContext(file_script, sandbox);

  return sandbox.module.exports;
};

const publicRequire = function (filename) {
  let ret = requireFileFromJsEntry(filename);
  if (!ret) {
    ret = requireFileFromProcess(filename);
  }
  return ret;
};

globalThis.require = publicRequire;
