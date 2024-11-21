/**
 * Adaptive Changes：
 * 1. export timeToSampleFrame grainLengthInSampleFrames to global
 *    - timeToSampleFrame grainLengthInSampleFrames are used in note-grain-on-testing.js
 */

const testName = "wptAudioBufferSourceNodeTest10<note-grain-on-timing.js>";
const {
  waitUntilTestComplete,
  requireReload,
} = require("js_tests/wpt_test/helper/polyfile.js");

async function wptAudioBufferSourceNodeTest10() {
  const { getDomException } = require("js_tests/wpt_test/helper/polyfile.js");
  globalThis.self = globalThis.window = {};

  requireReload("js_tests/wpt_test/helper/testharness.js");
  requireReload("js_tests/wpt_test/helper/testharnessreport.js");
  requireReload("js_tests/wpt_test/helper/audit.js");
  const {
    timeToSampleFrame,
    grainLengthInSampleFrames,
  } = require("js_tests/wpt_test/helper/audit-util.js");
  const {
    sampleRate,
    numberOfTests,
    renderTime,
    createSignalBuffer,
    findStartAndEndSamples,
    playAllGrains,
    verifyStartAndEndFrames,
  } = require("js_tests/wpt_test/helper/note-grain-on-testing.js");
  globalThis.timeToSampleFrame = timeToSampleFrame;
  globalThis.grainLengthInSampleFrames = grainLengthInSampleFrames;

  Object.assign(globalThis, globalThis.self);
  globalThis.DOMException = getDomException();

  let audit = Audit.createTaskRunner();

  let squarePulseBuffer;

  function checkResult(buffer, should) {
    renderedData = buffer.getChannelData(0);
    let nSamples = renderedData.length;
    let startEndFrames = findStartAndEndSamples(renderedData);

    verifyStartAndEndFrames(startEndFrames, should);
  }

  audit.define("Test timing of noteGrainOn", function (task, should) {
    // Create offline audio context.
    context = new OfflineAudioContext(2, sampleRate * renderTime, sampleRate);

    squarePulseBuffer = createSignalBuffer(context, function (k) {
      return 1;
    });

    playAllGrains(context, squarePulseBuffer, numberOfTests);

    context.startRendering().then(function (audioBuffer) {
      checkResult(audioBuffer, should);
      task.done();
    });
  });

  audit.run();

  return waitUntilTestComplete(globalThis, testName);
}

module.exports = { wptAudioBufferSourceNodeTest10 };
