/**
 * Adaptive Changes：
 * 1. use RangeError instead of NotSupportedError
 *    - nodejs has not NotSupportedError
 */

const testName = "wptDelayNodeTest4<delaynode-maxdelaylimit.js>";
const {
  waitUntilTestComplete,
  requireReload,
} = require("js_tests/wpt_test/helper/polyfile.js");

async function wptDelayNodeTest4() {
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
      description: "Tests attribute and maximum allowed delay of DelayNode",
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

      window.context = context;
      should(
        () => context.createDelay(180),
        "Setting Delay length to 180 seconds or more"
      ).throw(DOMException, "RangeError");
      should(
        () => context.createDelay(0),
        "Setting Delay length to 0 seconds"
      ).throw(DOMException, "RangeError");
      should(
        () => context.createDelay(-1),
        "Setting Delay length to negative"
      ).throw(DOMException, "RangeError");
      should(
        () => context.createDelay(NaN),
        "Setting Delay length to NaN"
      ).throw(TypeError);

      let delay = context.createDelay(179);
      delay.delayTime.value = delayTimeSeconds;
      window.delay = delay;
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
        .then(() => task.done());
    }
  );

  audit.run();

  return waitUntilTestComplete(globalThis, testName);
}

module.exports = { wptDelayNodeTest4 };
