/**
 * Adaptive Changes：
 * 1. use Error instead of InvalidStateError
 *    - nodejs has not InvalidStateError
 */

const testName = "wptAudioBufferSourceNodeTest2<audiobuffersource-channels.js>";
const {
  waitUntilTestComplete,
  requireReload,
} = require("js_tests/wpt_test/helper/polyfile.js");

async function wptAudioBufferSourceNodeTest2() {
  const { getDomException } = require("js_tests/wpt_test/helper/polyfile.js");
  globalThis.self = globalThis.window = {};

  requireReload("js_tests/wpt_test/helper/testharness.js");
  requireReload("js_tests/wpt_test/helper/testharnessreport.js");
  requireReload("js_tests/wpt_test/helper/audit-util.js");
  requireReload("js_tests/wpt_test/helper/audit.js");

  Object.assign(globalThis, globalThis.self);
  globalThis.DOMException = getDomException();

  let audit = Audit.createTaskRunner();
  let context;
  let source;

  audit.define(
    {
      label: "validate .buffer",
      description: "Validatation of AudioBuffer in .buffer attribute setter",
    },
    function (task, should) {
      context = new AudioContext();
      source = context.createBufferSource();

      // Make sure we can't set to something which isn't an AudioBuffer.
      should(function () {
        source.buffer = 57;
      }, "source.buffer = 57").throw(TypeError);

      // It's ok to set the buffer to null.
      should(function () {
        source.buffer = null;
      }, "source.buffer = null").notThrow();

      // Set the buffer to a valid AudioBuffer
      let buffer = new AudioBuffer({
        length: 128,
        sampleRate: context.sampleRate,
      });

      should(function () {
        source.buffer = buffer;
      }, "source.buffer = buffer").notThrow();

      // The buffer has been set; we can't set it again.
      should(function () {
        source.buffer = new AudioBuffer({
          length: 128,
          sampleRate: context.sampleRate,
        });
        // }, "source.buffer = new buffer").throw(DOMException, "InvalidStateError");
      }, "source.buffer = new buffer").throw(DOMException, "Error");

      // The buffer has been set; it's ok to set it to null.
      should(function () {
        source.buffer = null;
      }, "source.buffer = null again").notThrow();

      // The buffer was already set (and set to null).  Can't set it
      // again.
      should(function () {
        source.buffer = buffer;
      }, "source.buffer = buffer again").throw(
        DOMException,
        // "InvalidStateError"
        "Error"
      );

      // But setting to null is ok.
      should(function () {},
      "source.buffer = null after setting to null").notThrow();

      // Check that mono buffer can be set.
      should(function () {
        let monoBuffer = context.createBuffer(1, 1024, context.sampleRate);
        let testSource = context.createBufferSource();
        testSource.buffer = monoBuffer;
      }, "Setting source with mono buffer").notThrow();

      // Check that stereo buffer can be set.
      should(function () {
        let stereoBuffer = context.createBuffer(2, 1024, context.sampleRate);
        let testSource = context.createBufferSource();
        testSource.buffer = stereoBuffer;
      }, "Setting source with stereo buffer").notThrow();

      // Check buffers with more than two channels.
      for (let i = 3; i < 10; ++i) {
        should(function () {
          let buffer = context.createBuffer(i, 1024, context.sampleRate);
          let testSource = context.createBufferSource();
          testSource.buffer = buffer;
        }, "Setting source with " + i + " channels buffer").notThrow();
      }
      task.done();
    }
  );

  audit.run();

  return waitUntilTestComplete(globalThis, testName);
}

module.exports = { wptAudioBufferSourceNodeTest2 };
