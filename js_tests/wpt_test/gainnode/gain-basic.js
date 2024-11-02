const testName = "wptGainNodeTest1<gain-basic.js>";
const {
  waitUntilTestComplete,
  requireReload,
} = require("js_tests/wpt_test/helper/polyfile.js");

function wptGainNodeTest1() {
  const { getDomException } = require("js_tests/wpt_test/helper/polyfile.js");
  globalThis.self = globalThis.window = {};

  requireReload("js_tests/wpt_test/helper/testharness.js");
  requireReload("js_tests/wpt_test/helper/testharnessreport.js");
  requireReload("js_tests/wpt_test/helper/audit.js");

  Object.assign(globalThis, globalThis.self);
  globalThis.DOMException = getDomException();

  let audit = Audit.createTaskRunner();

  audit.define("test", function (task, should) {
    // Create audio context.
    let context = new AudioContext();

    // Create gain node.
    let gainNode = context.createGain();

    should(
      gainNode.gain instanceof AudioParam,
      "gainNode.gain instanceof AudioParam"
    ).beTrue();

    task.done();
  });

  audit.run();

  return waitUntilTestComplete(globalThis, testName);
}

module.exports = { wptGainNodeTest1 };
