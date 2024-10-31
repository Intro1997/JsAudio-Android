/**
 * Adaptive Changes：
 * 1. use RangeError to replace IndexSizeError
 *    - nodejs has not IndexSizeError
 */

const testName = "wptOscillatorTest2<detune-overflow.js>";
const {
  waitUntilTestComplete,
  requireReload,
} = require("js_tests/wpt_test/helper/polyfile.js");

function wptOscillatorTest2() {
  const { getDomException } = require("js_tests/wpt_test/helper/polyfile.js");
  globalThis.self = globalThis.window = {};

  requireReload("js_tests/wpt_test/helper/testharness.js");
  requireReload("js_tests/wpt_test/helper/testharnessreport.js");
  requireReload("js_tests/wpt_test/helper/audit.js");
  const {
    RENDER_QUANTUM_FRAMES,
  } = require("js_tests/wpt_test/helper/audit-util.js");

  Object.assign(globalThis, globalThis.self);
  globalThis.DOMException = getDomException();
  const sampleRate = 44100;
  const renderLengthFrames = RENDER_QUANTUM_FRAMES;

  let audit = Audit.createTaskRunner();

  audit.define("detune overflow", async (task, should) => {
    let context = new OfflineAudioContext(1, renderLengthFrames, sampleRate);

    // This value of frequency and detune results in a computed frequency of
    // 440*2^(153600/1200) = 1.497e41.  The frequency needs to be clamped to
    // Nyquist.  But a sine wave at Nyquist frequency is all zeroes.  Verify
    // the output is 0.
    let osc = new OscillatorNode(context, { frequency: 440, detune: 153600 });

    osc.connect(context.destination);

    let buffer = await context.startRendering();
    let output = buffer.getChannelData(0);
    should(
      output,
      "Osc freq and detune outside nominal range"
    ).beConstantValueOf(0);

    task.done();
  });

  audit.run();

  return waitUntilTestComplete(globalThis, testName);
}

module.exports = { wptOscillatorTest2 };
