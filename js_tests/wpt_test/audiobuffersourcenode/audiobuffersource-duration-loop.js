/**
 * Adaptive Changes：
 * 1. move `task.done()` after should() call
 *    - task.done() will be call before should() end,
 *      in this case, add_completion_callback() will be
 *      called before test end. So we put task.done()
 *      after should() call.
 */

const testName =
  "wptAudioBufferSourceNodeTest3<audiobuffersource-duration-loop.js>";
const {
  waitUntilTestComplete,
  requireReload,
} = require("js_tests/wpt_test/helper/polyfile.js");

async function wptAudioBufferSourceNodeTest3() {
  const { getDomException } = require("js_tests/wpt_test/helper/polyfile.js");
  globalThis.self = globalThis.window = {};

  requireReload("js_tests/wpt_test/helper/testharness.js");
  requireReload("js_tests/wpt_test/helper/testharnessreport.js");
  requireReload("js_tests/wpt_test/helper/audit-util.js");
  requireReload("js_tests/wpt_test/helper/audit.js");

  Object.assign(globalThis, globalThis.self);
  globalThis.DOMException = getDomException();

  let audit = Audit.createTaskRunner();
  audit.define("loop with duration", (task, should) => {
    // Create the context
    let context = new OfflineAudioContext(1, 4096, 48000);

    // Create the sample buffer and fill the second half with 1
    let buffer = context.createBuffer(1, 2048, context.sampleRate);
    for (let i = 1024; i < 2048; i++) {
      buffer.getChannelData(0)[i] = 1;
    }

    // Create the source and set its value
    let source = context.createBufferSource();
    source.loop = true;
    source.loopStart = 1024 / context.sampleRate;
    source.loopEnd = 2048 / context.sampleRate;
    source.buffer = buffer;
    source.connect(context.destination);
    source.start(0, 1024 / context.sampleRate, 2048 / context.sampleRate);
    // Expectations
    let expected = new Float32Array(4096);
    for (let i = 0; i < 2048; i++) {
      expected[i] = 1;
    }
    // Render it!
    context
      .startRendering()
      .then(function (audioBuffer) {
        should(
          audioBuffer.getChannelData(0),
          "audioBuffer.getChannelData"
        ).beEqualToArray(expected);
      })
      .then(task.done.bind(task));
  });

  audit.run();

  return waitUntilTestComplete(globalThis, testName);
}

module.exports = { wptAudioBufferSourceNodeTest3 };
