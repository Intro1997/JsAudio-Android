const testName = "OfflineAudioContextTest1";

const {
  waitUntilTestComplete,
  requireReload,
} = require("js_tests/wpt_test/helper/polyfile.js");

function wptOfflineAudioContextTest1() {
  {
    const { getDomException } = require("js_tests/wpt_test/helper/polyfile.js");
    globalThis.self = globalThis.window = {};

    requireReload("js_tests/wpt_test/helper/testharness.js");
    requireReload("js_tests/wpt_test/helper/testharnessreport.js");

    Object.assign(globalThis, globalThis.self);
    globalThis.DOMException = getDomException();
  }

  promise_test(function () {
    var context = new OfflineAudioContext(1, 1, 65536);
    return context.startRendering().then(function (buffer) {
      assert_equals(buffer.length, 1, "buffer length");
      assert_equals(
        context.currentTime,
        128 / context.sampleRate,
        "currentTime at completion"
      );
    });
  });

  return waitUntilTestComplete(globalThis, testName);
}

module.exports = { wptOfflineAudioContextTest1 };
