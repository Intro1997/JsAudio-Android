/**
 * Adaptive Changes：
 * 1. export timeToSampleFrame grainLengthInSampleFrames to global
 *    - timeToSampleFrame grainLengthInSampleFrames are used in note-grain-on-testing.js
 */

const testName =
  "wptAudioBufferSourceNodeTest11<sample-accurate-scheduling.js>";
const {
  waitUntilTestComplete,
  requireReload,
} = require("js_tests/wpt_test/helper/polyfile.js");

async function wptAudioBufferSourceNodeTest11() {
  const { getDomException } = require("js_tests/wpt_test/helper/polyfile.js");
  globalThis.self = globalThis.window = {};

  requireReload("js_tests/wpt_test/helper/testharness.js");
  requireReload("js_tests/wpt_test/helper/testharnessreport.js");
  requireReload("js_tests/wpt_test/helper/audit.js");

  Object.assign(globalThis, globalThis.self);
  globalThis.DOMException = getDomException();

  let audit = Audit.createTaskRunner();

  let sampleRate = 44100.0;
  let lengthInSeconds = 4;

  let context = 0;
  let bufferLoader = 0;
  let impulse;

  // See if we can render at exactly these sample offsets.
  let sampleOffsets = [0, 3, 512, 517, 1000, 1005, 20000, 21234, 37590];

  function createImpulse() {
    // An impulse has a value of 1 at time 0, and is otherwise 0.
    impulse = context.createBuffer(2, 512, sampleRate);
    let sampleDataL = impulse.getChannelData(0);
    let sampleDataR = impulse.getChannelData(1);
    sampleDataL[0] = 1.0;
    sampleDataR[0] = 1.0;
  }

  function playNote(time) {
    let bufferSource = context.createBufferSource();
    bufferSource.buffer = impulse;
    bufferSource.connect(context.destination);
    bufferSource.start(time);
  }

  function checkSampleAccuracy(buffer, should) {
    let bufferDataL = buffer.getChannelData(0);
    let bufferDataR = buffer.getChannelData(1);

    let impulseCount = 0;
    let badOffsetCount = 0;

    // Left and right channels must be the same.
    should(
      bufferDataL,
      "Content of left and right channels match and"
    ).beEqualToArray(bufferDataR);

    // Go through every sample and make sure it's 0, except at positions in
    // sampleOffsets.
    for (let i = 0; i < buffer.length; ++i) {
      if (bufferDataL[i] != 0) {
        // Make sure this index is  in sampleOffsets
        let found = false;
        for (let j = 0; j < sampleOffsets.length; ++j) {
          if (sampleOffsets[j] == i) {
            found = true;
            break;
          }
        }
        ++impulseCount;
        should(found, "Non-zero sample found at sample offset " + i).beTrue();
        if (!found) {
          ++badOffsetCount;
        }
      }
    }

    should(impulseCount, "Number of impulses found").beEqualTo(
      sampleOffsets.length
    );

    if (impulseCount == sampleOffsets.length) {
      should(badOffsetCount, "bad offset").beEqualTo(0);
    }
  }

  audit.define(
    { label: "test", description: "Test sample-accurate scheduling" },
    function (task, should) {
      // Create offline audio context.
      context = new OfflineAudioContext(
        2,
        sampleRate * lengthInSeconds,
        sampleRate
      );
      createImpulse();

      for (let i = 0; i < sampleOffsets.length; ++i) {
        let timeInSeconds = sampleOffsets[i] / sampleRate;
        playNote(timeInSeconds);
      }

      context.startRendering().then(function (buffer) {
        checkSampleAccuracy(buffer, should);
        task.done();
      });
    }
  );

  audit.run();

  return waitUntilTestComplete(globalThis, testName);
}

module.exports = { wptAudioBufferSourceNodeTest11 };
