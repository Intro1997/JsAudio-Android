const testName =
  "wptAudioBufferSourceNodeTest6<audiobuffersource-one-sample-loop.js>";
const {
  waitUntilTestComplete,
  requireReload,
} = require("js_tests/wpt_test/helper/polyfile.js");

async function wptAudioBufferSourceNodeTest6() {
  const { getDomException } = require("js_tests/wpt_test/helper/polyfile.js");
  globalThis.self = globalThis.window = {};

  requireReload("js_tests/wpt_test/helper/testharness.js");
  requireReload("js_tests/wpt_test/helper/testharnessreport.js");
  const {
    createConstantBuffer,
  } = require("js_tests/wpt_test/helper/audit-util.js");
  requireReload("js_tests/wpt_test/helper/audit.js");

  Object.assign(globalThis, globalThis.self);
  globalThis.DOMException = getDomException();

  let audit = Audit.createTaskRunner();

  let sampleRate = 44100;
  let testDurationSamples = 1000;

  audit.define("one-sample-loop", function (task, should) {
    // Create the offline context for the test.
    let context = new OfflineAudioContext(1, testDurationSamples, sampleRate);

    // Create the single sample buffer
    let buffer = createConstantBuffer(context, 1, 1);

    // Create the source and connect it to the destination
    let source = context.createBufferSource();
    source.buffer = buffer;
    source.loop = true;
    source.connect(context.destination);
    source.start();

    // Render it!
    context
      .startRendering()
      .then(function (audioBuffer) {
        should(
          audioBuffer.getChannelData(0),
          "Rendered data"
        ).beConstantValueOf(1);
      })
      .then(task.done.bind(task));
  });

  audit.run();

  return waitUntilTestComplete(globalThis, testName);
}

module.exports = { wptAudioBufferSourceNodeTest6 };
