globalThis.native_log = process._linkedBinding("node_logger").log;

const Writable = require("stream").Writable;
const Console = require("console").Console;

const native_stdout = new Writable({
  write: (data, _, cb) => {
    let out_string = "";

    if (Buffer.isBuffer(data)) {
      out_string = data.toString();
    } else if (typeof data === "string") {
      out_string = data;
    } else {
      out_string = "error: Unknown write data type!";
    }

    native_log(out_string, native_stdout.console_level);

    if (cb instanceof Function) {
      cb();
    }
  },
});

native_stdout.console_level = 0;

const my_console = new Console({
  stdout: native_stdout,
  stderr: native_stdout,
});

my_console._log = my_console.log;
my_console._warn = my_console.warn;
my_console._error = my_console.error;
my_console._info = my_console.info;

my_console.log = (...args) => {
  native_stdout.console_level = 0;
  my_console._log(...args);
};
my_console.info = (...args) => {
  native_stdout.console_level = 1;
  my_console._info(...args);
};
my_console.warn = (...args) => {
  native_stdout.console_level = 2;
  my_console._warn(...args);
};
my_console.error = (...args) => {
  native_stdout.console_level = 3;
  my_console._error(...args);
};
console.log = my_console.log;
console.info = my_console.info;
console.warn = my_console.warn;
console.error = my_console.error;
