/**
 * Adaptive Changes：
 * 1. use RangeError to replace IndexSizeError
 *    - nodejs has not IndexSizeError
 */

const testName = "wptAudioBufferTest0<audiobuffer.js>";
const {
  waitUntilTestComplete,
  requireReload,
} = require("js_tests/wpt_test/helper/polyfile.js");

function wptAudioBufferTest0() {
  const { getDomException } = require("js_tests/wpt_test/helper/polyfile.js");
  globalThis.self = globalThis.window = {};

  requireReload("js_tests/wpt_test/helper/testharness.js");
  requireReload("js_tests/wpt_test/helper/testharnessreport.js");
  requireReload("js_tests/wpt_test/helper/audit-util.js");
  requireReload("js_tests/wpt_test/helper/audit.js");

  Object.assign(globalThis, globalThis.self);
  globalThis.DOMException = getDomException();
  globalThis.window.Float32Array = Float32Array;

  let sampleRate = 44100.0;
  let lengthInSeconds = 2;
  let numberOfChannels = 4;

  let audit = Audit.createTaskRunner();

  audit.define("Basic tests for AudioBuffer", function (task, should) {
    let context = new AudioContext();
    let buffer = context.createBuffer(
      numberOfChannels,
      sampleRate * lengthInSeconds,
      sampleRate
    );

    // Just for printing out a message describing what "buffer" is in the
    // following tests.
    should(
      true,
      "buffer = context.createBuffer(" +
        numberOfChannels +
        ", " +
        sampleRate * lengthInSeconds +
        ", " +
        sampleRate +
        ")"
    ).beTrue();

    should(buffer.sampleRate, "buffer.sampleRate").beEqualTo(sampleRate);

    should(buffer.length, "buffer.length").beEqualTo(
      sampleRate * lengthInSeconds
    );

    should(buffer.duration, "buffer.duration").beEqualTo(lengthInSeconds);

    should(buffer.numberOfChannels, "buffer.numberOfChannels").beEqualTo(
      numberOfChannels
    );

    for (let index = 0; index < buffer.numberOfChannels; ++index) {
      should(
        buffer.getChannelData(index) instanceof window.Float32Array,
        "buffer.getChannelData(" + index + ") instanceof window.Float32Array"
      ).beTrue();
    }

    should(function () {
      buffer.getChannelData(buffer.numberOfChannels);
    }, "buffer.getChannelData(" + buffer.numberOfChannels + ")").throw(
      DOMException,
      // "IndexSizeError"
      "RangeError"
    );

    let buffer2 = context.createBuffer(1, 1000, 24576);
    let expectedDuration = 1000 / 24576;

    should(
      buffer2.duration,
      "context.createBuffer(1, 1000, 24576).duration"
    ).beEqualTo(expectedDuration);

    task.done();
  });

  audit.run();
  return waitUntilTestComplete(globalThis, testName);
}

module.exports = { wptAudioBufferTest0 };
