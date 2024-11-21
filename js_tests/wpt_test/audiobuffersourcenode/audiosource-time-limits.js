const testName = "wptAudioBufferSourceNodeTest8<audiosource-time-limits.js>";
const {
  waitUntilTestComplete,
  requireReload,
} = require("js_tests/wpt_test/helper/polyfile.js");

async function wptAudioBufferSourceNodeTest8() {
  const { getDomException } = require("js_tests/wpt_test/helper/polyfile.js");
  globalThis.self = globalThis.window = {};

  requireReload("js_tests/wpt_test/helper/testharness.js");
  requireReload("js_tests/wpt_test/helper/testharnessreport.js");
  requireReload("js_tests/wpt_test/helper/audit.js");
  const {
    createConstantBuffer,
  } = require("js_tests/wpt_test/helper/audit-util.js");
  requireReload("js_tests/wpt_test/helper/audioparam-testing.js");

  Object.assign(globalThis, globalThis.self);
  globalThis.DOMException = getDomException();
  let sampleRate = 48000;
  let renderFrames = 1000;

  let audit = Audit.createTaskRunner();

  audit.define("buffersource: huge stop time", (task, should) => {
    // We only need to generate a small number of frames for this test.
    let context = new OfflineAudioContext(1, renderFrames, sampleRate);
    let src = context.createBufferSource();

    // Constant source of amplitude 1, looping.
    src.buffer = createConstantBuffer(context, 1, 1);
    src.loop = true;

    // Create the graph and go!
    let endTime = 1e300;
    src.connect(context.destination);
    src.start();
    src.stop(endTime);

    context
      .startRendering()
      .then(function (resultBuffer) {
        let result = resultBuffer.getChannelData(0);
        should(
          result,
          "Output from AudioBufferSource.stop(" + endTime + ")"
        ).beConstantValueOf(1);
      })
      .then(() => task.done());
  });

  audit.define("oscillator: huge stop time", (task, should) => {
    // We only need to generate a small number of frames for this test.
    let context = new OfflineAudioContext(1, renderFrames, sampleRate);
    let src = context.createOscillator();

    // Create the graph and go!
    let endTime = 1e300;
    src.connect(context.destination);
    src.start();
    src.stop(endTime);

    context
      .startRendering()
      .then(function (resultBuffer) {
        let result = resultBuffer.getChannelData(0);
        // The buffer should not be empty.  Just find the max and verify
        // that it's not zero.
        let max = Math.max.apply(null, result);
        should(
          max,
          "Peak amplitude from oscillator.stop(" + endTime + ")"
        ).beGreaterThan(0);
      })
      .then(() => task.done());
  });

  audit.run();

  return waitUntilTestComplete(globalThis, testName);
}

module.exports = { wptAudioBufferSourceNodeTest8 };
