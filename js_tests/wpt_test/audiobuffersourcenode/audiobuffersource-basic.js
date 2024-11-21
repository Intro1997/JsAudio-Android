const testName = "wptAudioBufferSourceNodeTest1<audiobuffersource-basic.js>";
const {
  waitUntilTestComplete,
  requireReload,
} = require("js_tests/wpt_test/helper/polyfile.js");

async function wptAudioBufferSourceNodeTest1() {
  const { getDomException } = require("js_tests/wpt_test/helper/polyfile.js");
  globalThis.self = globalThis.window = {};

  const {
    testStartStop,
  } = require("js_tests/wpt_test/helper/start-stop-exceptions.js");
  requireReload("js_tests/wpt_test/helper/testharness.js");
  requireReload("js_tests/wpt_test/helper/testharnessreport.js");
  requireReload("js_tests/wpt_test/helper/audit-util.js");
  requireReload("js_tests/wpt_test/helper/audit.js");

  Object.assign(globalThis, globalThis.self);
  globalThis.DOMException = getDomException();

  let sampleRate = 44100;
  let renderLengthSeconds = 0.25;

  let oscTypes = ["sine", "square", "sawtooth", "triangle", "custom"];

  let audit = Audit.createTaskRunner();

  audit.define("start/stop exceptions", (task, should) => {
    // We're not going to render anything, so make it simple
    let context = new OfflineAudioContext(1, 1, sampleRate);
    let node = new AudioBufferSourceNode(context);

    testStartStop(should, node, [
      { args: [0, -1], errorType: RangeError },
      { args: [0, 0, -1], errorType: RangeError },
    ]);
    task.done();
  });

  audit.run();

  return waitUntilTestComplete(globalThis, testName);
}

module.exports = { wptAudioBufferSourceNodeTest1 };
