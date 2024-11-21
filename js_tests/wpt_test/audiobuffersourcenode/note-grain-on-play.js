/**
 * Adaptive Changes：
 * 1. export timeToSampleFrame grainLengthInSampleFrames to global
 *    - timeToSampleFrame grainLengthInSampleFrames are used in note-grain-on-testing.js
 */

const testName = "wptAudioBufferSourceNodeTest9<note-grain-on-play.js>";
const {
  waitUntilTestComplete,
  requireReload,
} = require("js_tests/wpt_test/helper/polyfile.js");

async function wptAudioBufferSourceNodeTest9() {
  const { getDomException } = require("js_tests/wpt_test/helper/polyfile.js");
  globalThis.self = globalThis.window = {};

  requireReload("js_tests/wpt_test/helper/testharness.js");
  requireReload("js_tests/wpt_test/helper/testharnessreport.js");
  requireReload("js_tests/wpt_test/helper/audit.js");
  const {
    timeToSampleFrame,
    grainLengthInSampleFrames,
  } = require("js_tests/wpt_test/helper/audit-util.js");
  globalThis.timeToSampleFrame = timeToSampleFrame;
  globalThis.grainLengthInSampleFrames = grainLengthInSampleFrames;
  const {
    sampleRate,
    numberOfTests,
    renderTime,
    createSignalBuffer,
    findStartAndEndSamples,
    playAllGrains,
    verifyStartAndEndFrames,
  } = require("js_tests/wpt_test/helper/note-grain-on-testing.js");

  Object.assign(globalThis, globalThis.self);
  globalThis.DOMException = getDomException();

  let audit = Audit.createTaskRunner();

  // To test noteGrainOn, a single ramp signal is created.
  // Various sections of the ramp are rendered by noteGrainOn() at
  // different times, and we verify that the actual output
  // consists of the correct section of the ramp at the correct
  // time.

  let linearRampBuffer;

  // Array of the grain offset used for each ramp played.
  let grainOffsetTime = [];

  // Verify the received signal is a ramp from the correct section
  // of our ramp signal.
  function verifyGrain(renderedData, startFrame, endFrame, grainIndex) {
    let grainOffsetFrame = timeToSampleFrame(
      grainOffsetTime[grainIndex],
      sampleRate
    );
    let grainFrameLength = endFrame - startFrame;
    let ramp = linearRampBuffer.getChannelData(0);
    let isCorrect = true;

    let expected;
    let actual;
    let frame;

    for (let k = 0; k < grainFrameLength; ++k) {
      if (renderedData[startFrame + k] != ramp[grainOffsetFrame + k]) {
        expected = ramp[grainOffsetFrame + k];
        actual = renderedData[startFrame + k];
        frame = startFrame + k;
        isCorrect = false;
        break;
      }
    }
    return {
      verified: isCorrect,
      expected: expected,
      actual: actual,
      frame: frame,
    };
  }

  function checkResult(buffer, should) {
    renderedData = buffer.getChannelData(0);
    let nSamples = renderedData.length;

    // Number of grains that we found that have incorrect data.
    let invalidGrainDataCount = 0;

    let startEndFrames = findStartAndEndSamples(renderedData);

    // Verify the start and stop times.  Not strictly needed for
    // this test, but it's useful to know that if the ramp data
    // appears to be incorrect.
    verifyStartAndEndFrames(startEndFrames, should);

    // Loop through each of the rendered grains and check that
    // each grain contains our expected ramp.
    for (let k = 0; k < startEndFrames.start.length; ++k) {
      // Verify that the rendered data matches the expected
      // section of our ramp signal.
      let result = verifyGrain(
        renderedData,
        startEndFrames.start[k],
        startEndFrames.end[k],
        k
      );
      should(
        result.verified,
        "Pulse " + k + " contained the expected data"
      ).beTrue();
    }
    should(
      invalidGrainDataCount,
      "Number of grains that did not contain the expected data"
    ).beEqualTo(0);
  }

  audit.define(
    {
      label: "note-grain-on-play",
      description: "Test noteGrainOn offset rendering",
    },
    function (task, should) {
      // Create offline audio context.
      context = new OfflineAudioContext(2, sampleRate * renderTime, sampleRate);

      // Create a linear ramp for testing noteGrainOn.
      linearRampBuffer = createSignalBuffer(context, function (k) {
        // Want the ramp to start
        // with 1, not 0.
        return k + 1;
      });

      // numberOfTests = 100
      let grainInfo = playAllGrains(context, linearRampBuffer, numberOfTests);

      grainOffsetTime = grainInfo.grainOffsetTimes;

      context.startRendering().then(function (audioBuffer) {
        checkResult(audioBuffer, should);
        task.done();
      });
    }
  );

  audit.run();

  return waitUntilTestComplete(globalThis, testName);
}

module.exports = { wptAudioBufferSourceNodeTest9 };
