/**
 * Adaptive Changes：
 * 1. use Error instead of InvalidStateError
 *    - nodejs has not InvalidStateError
 */

const testName = "wptAudioBufferSourceNodeTest4<audiobuffersource-grain.js>";
const {
  waitUntilTestComplete,
  requireReload,
} = require("js_tests/wpt_test/helper/polyfile.js");

async function wptAudioBufferSourceNodeTest4() {
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
  let context;
  let source;
  let buffer;
  let renderedData;

  let sampleRate = 44100;

  let testDurationSec = 1;
  let testDurationSamples = testDurationSec * sampleRate;
  let startTime = 0.9 * testDurationSec;

  audit.define(
    "Test setting the source buffer after starting the grain",
    function (task, should) {
      context = new OfflineAudioContext(1, testDurationSamples, sampleRate);

      buffer = createConstantBuffer(context, testDurationSamples, 1);
      source = context.createBufferSource();
      source.connect(context.destination);

      // Start the source BEFORE we set the buffer. The grain offset and
      // duration aren't important, as long as we specify some offset.
      source.start(startTime, 0.1);
      source.buffer = buffer;

      // Render it!
      context
        .startRendering()
        .then(function (buffer) {
          const bufferArr = buffer.getChannelData(0);
          checkResult(buffer, should);
        })
        .then(task.done.bind(task));
    }
  );

  function checkResult(buffer, should) {
    let success = false;

    renderedData = buffer.getChannelData(0);

    // Check that the rendered data is not all zeroes.  Any non-zero data
    // means the test passed.
    let startFrame = Math.round(startTime * sampleRate);
    for (k = 0; k < renderedData.length; ++k) {
      if (renderedData[k]) {
        success = true;
        break;
      }
    }

    should(success, "Buffer was played").beTrue();
  }

  audit.run();

  return waitUntilTestComplete(globalThis, testName);
}

module.exports = { wptAudioBufferSourceNodeTest4 };
