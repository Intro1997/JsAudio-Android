const testName = "wptDelayNodeTest2<delaynode-max-nondefault-delay.js>";
const {
  waitUntilTestComplete,
  requireReload,
} = require("js_tests/wpt_test/helper/polyfile.js");

async function wptDelayNodeTest2() {
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
      description:
        "Basic functionality of DelayNode with a non-default max delay time",
    },
    function (task, should) {
      // Create offline audio context.
      let context = new OfflineAudioContext(
        1,
        sampleRate * renderLengthSeconds,
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

      // Create a delay node with an explicit max delay time (greater than
      // the default of 1 second).
      let delay = context.createDelay(2);
      // Set the delay time to a value greater than the default max delay
      // so we can verify the delay is working for this case.
      delayTimeSeconds = 1.5;
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

module.exports = { wptDelayNodeTest2 };
