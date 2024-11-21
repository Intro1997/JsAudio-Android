/**
 * Adaptive Changes：
 * 1. use RangeError instead of NotSupportedError
 *    - nodejs has not NotSupportedError
 */

const testName = "wptDelayNodeTest5<delaynode.js>";
const {
  waitUntilTestComplete,
  requireReload,
} = require("js_tests/wpt_test/helper/polyfile.js");

async function wptDelayNodeTest5() {
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
      description: "Tests attribute and basic functionality of DelayNode",
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

      let delay = context.createDelay();

      window.delay = delay;
      should(delay.numberOfInputs, "delay.numberOfInputs").beEqualTo(1);
      should(delay.numberOfOutputs, "delay.numberOfOutputs").beEqualTo(1);
      should(
        delay.delayTime.defaultValue,
        "delay.delayTime.defaultValue"
      ).beEqualTo(0.0);
      should(delay.delayTime.value, "delay.delayTime.value").beEqualTo(0.0);

      delay.delayTime.value = delayTimeSeconds;
      should(
        delay.delayTime.value,
        "delay.delayTime.value = " + delayTimeSeconds
      ).beEqualTo(delayTimeSeconds);

      bufferSource.connect(delay);
      delay.connect(context.destination);
      bufferSource.start(0);

      context
        .startRendering()
        .then((buffer) => checkDelayedResult(buffer, toneBuffer, should))
        .then(task.done.bind(task));
    }
  );

  audit.run();

  return waitUntilTestComplete(globalThis, testName);
}

module.exports = { wptDelayNodeTest5 };
