const testName = "wptOscillatorTest<audiobuffer-getChannelData.js>";

const {
  waitUntilTestComplete,
  requireReload,
} = require("js_tests/wpt_test/helper/polyfile.js");

function wptAudioBufferTest2() {
  const { getDomException } = require("js_tests/wpt_test/helper/polyfile.js");
  globalThis.self = globalThis.window = {};

  requireReload("js_tests/wpt_test/helper/testharness.js");
  requireReload("js_tests/wpt_test/helper/testharnessreport.js");
  requireReload("js_tests/wpt_test/helper/audit.js");
  requireReload("js_tests/wpt_test/helper/audioparam-testing.js");

  Object.assign(globalThis, globalThis.self);
  globalThis.DOMException = getDomException();
  let sampleRate = 48000;
  let renderDuration = 0.5;

  let audit = Audit.createTaskRunner();

  audit.define("buffer-eq", (task, should) => {
    // Verify that successive calls to getChannelData return the same
    // buffer.
    let context = new AudioContext();
    let channelCount = 2;
    let frameLength = 1000;
    let buffer = context.createBuffer(
      channelCount,
      frameLength,
      context.sampleRate
    );

    for (let c = 0; c < channelCount; ++c) {
      let a = buffer.getChannelData(c);
      let b = buffer.getChannelData(c);

      let message = "buffer.getChannelData(" + c + ")";
      should(a === b, message + " === " + message).beEqualTo(true);
    }

    task.done();
  });

  audit.define("buffer-not-eq", (task, should) => {
    let context = new AudioContext();
    let channelCount = 2;
    let frameLength = 1000;
    let buffer1 = context.createBuffer(
      channelCount,
      frameLength,
      context.sampleRate
    );
    let buffer2 = context.createBuffer(
      channelCount,
      frameLength,
      context.sampleRate
    );
    let success = true;

    for (let c = 0; c < channelCount; ++c) {
      let a = buffer1.getChannelData(c);
      let b = buffer2.getChannelData(c);

      let message = "getChannelData(" + c + ")";
      should(
        a === b,
        "buffer1." + message + " === buffer2." + message
      ).beEqualTo(false) && success;
    }

    task.done();
  });

  audit.run();

  return waitUntilTestComplete(globalThis, testName);
}

module.exports = { wptAudioBufferTest2 };
