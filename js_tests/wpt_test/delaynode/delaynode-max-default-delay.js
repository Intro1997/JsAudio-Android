const testName = "wptDelayNodeTest1<delaynode-max-default-delay.js>";
const {
  waitUntilTestComplete,
  requireReload,
} = require("js_tests/wpt_test/helper/polyfile.js");

async function wptDelayNodeTest1() {
  const { getDomException } = require("js_tests/wpt_test/helper/polyfile.js");
  globalThis.self = globalThis.window = {};
  requireReload("js_tests/wpt_test/helper/testharness.js");
  requireReload("js_tests/wpt_test/helper/testharnessreport.js");
  requireReload("js_tests/wpt_test/helper/audit.js");

  const {
    sampleRate,
    renderLengthSeconds,
    toneLengthSeconds,
    createToneBuffer,
    checkDelayedResult,
  } = requireReload("js_tests/wpt_test/helper/delay-testing.js");

  Object.assign(globalThis, globalThis.self);
  globalThis.DOMException = getDomException();
  let audit = Audit.createTaskRunner();

  audit.define(
    {
      label: "test",
      description: "DelayNode with delay set to default maximum delay",
    },
    function (task, should) {
      // Create offline audio context.
      let context = new OfflineAudioContext(
        1,
        sampleRate * renderLengthSeconds, // 176400
        sampleRate
      );

      let toneBuffer = createToneBuffer(
        context,
        20,
        20 * toneLengthSeconds,
        sampleRate
      ); // 20Hz tone

      let bufferSource = context.createBufferSource();
      bufferSource.buffer = toneBuffer;

      let delay = context.createDelay();
      delayTimeSeconds = 1;
      delay.delayTime.value = delayTimeSeconds;

      bufferSource.connect(delay);
      delay.connect(context.destination);
      bufferSource.start(0);

      context
        .startRendering()
        .then((buffer) => checkDelayedResult(buffer, toneBuffer, should))
        .then(() => task.done());
    }
  );

  audit.run();
  return waitUntilTestComplete(globalThis, testName);
}

module.exports = { wptDelayNodeTest1 };
