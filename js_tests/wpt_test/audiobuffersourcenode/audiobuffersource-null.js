const testName = "wptAudioBufferSourceNodeTest5<audiobuffersource-null.js>";
const {
  waitUntilTestComplete,
  requireReload,
} = require("js_tests/wpt_test/helper/polyfile.js");

async function wptAudioBufferSourceNodeTest5() {
  const { getDomException } = require("js_tests/wpt_test/helper/polyfile.js");
  globalThis.self = globalThis.window = {};

  requireReload("js_tests/wpt_test/helper/testharness.js");
  requireReload("js_tests/wpt_test/helper/testharnessreport.js");
  requireReload("js_tests/wpt_test/helper/audit-util.js");
  requireReload("js_tests/wpt_test/helper/audit.js");

  Object.assign(globalThis, globalThis.self);
  globalThis.DOMException = getDomException();

  const audit = Audit.createTaskRunner();

  audit.define("ABSN with null buffer", (task, should) => {
    // Create test context.  Length and sampleRate are pretty arbitrary, but
    // we don't need either to be very large.
    const context = new OfflineAudioContext({
      numberOfChannels: 1,
      length: 1024,
      sampleRate: 8192,
    });

    // Just create a constant buffer for testing.  Anything will do as long
    // as the buffer contents are not identically zero.
    const audioBuffer = new AudioBuffer({
      length: 10,
      sampleRate: context.sampleRate,
    });
    const audioBufferSourceNode = new AudioBufferSourceNode(context);

    audioBuffer.getChannelData(0).fill(1);

    // These two tests are mostly for the informational messages to show
    // what's happening.  They should never fail!
    should(() => {
      audioBufferSourceNode.buffer = audioBuffer;
    }, "Setting ABSN.buffer to AudioBuffer").notThrow();

    // This is the important part.  Setting the buffer to null after setting
    // it to something else should cause the source to produce silence.
    should(() => {
      audioBufferSourceNode.buffer = null;
    }, "Setting ABSN.buffer = null").notThrow();

    audioBufferSourceNode.start(0);
    audioBufferSourceNode.connect(context.destination);

    context
      .startRendering()
      .then((buffer) => {
        // Since the buffer is null, the output of the source should be
        // silence.
        should(buffer.getChannelData(0), "ABSN output").beConstantValueOf(0);
      })
      .then(() => task.done());
  });

  audit.run();

  return waitUntilTestComplete(globalThis, testName);
}

module.exports = { wptAudioBufferSourceNodeTest5 };
