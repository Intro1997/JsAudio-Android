/**
 * Adaptive Changes：
 * 1. Use RangeError instead of DOMException here because:
 *    - all DOMException here are relative to range
 *    - NodeJS has RangeError
 */

// {
//   name:Executing "options-1"
//   message:promise_test: Unhandled rejection with value: object "TypeError: Cannot read property 'destination' of undefined"
// }

function wptOfflineAudioContextTest() {
  globalThis.self = globalThis.window = {};

  require("js_tests/wpt_test/helper/testharness.js");
  require("js_tests/wpt_test/helper/testharnessreport.js");
  require("js_tests/wpt_test/helper/audit.js");
  require("js_tests/wpt_test/helper/audit-util.js");
  require("js_tests/wpt_test/helper/audionodeoptions.js");

  const { retFormatter } = require("js_tests/wpt_test/helper/ret_formatter.js");
  const { getDomException } = require("js_tests/wpt_test/helper/polyfile.js");

  Object.assign(globalThis, globalThis.self);
  globalThis.DOMException = getDomException();

  let audit = Audit.createTaskRunner();

  // Just a simple test of the 3-arg constructor; This should be
  // well-covered by other layout tests that use the 3-arg constructor.
  audit.define(
    { label: "basic", description: "Old-style constructor" },
    (task, should) => {
      let context;

      // First and only arg should be a dictionary.
      should(() => {
        new OfflineAudioContext(3);
      }, "new OfflineAudioContext(3)").throw(TypeError);

      // Constructor needs 1 or 3 args, so 2 should throw.
      should(() => {
        new OfflineAudioContext(3, 42);
      }, "new OfflineAudioContext(3, 42)").throw(TypeError);

      // Valid constructor
      should(() => {
        context = new OfflineAudioContext(3, 42, 12345);
      }, "context = new OfflineAudioContext(3, 42, 12345)").notThrow();

      // Verify that the context was constructed correctly.
      should(context.length, "context.length").beEqualTo(42);
      should(context.sampleRate, "context.sampleRate").beEqualTo(12345);
      should(
        context.destination.channelCount,
        "context.destination.channelCount"
      ).beEqualTo(3);
      should(
        context.destination.channelCountMode,
        "context.destination.channelCountMode"
      ).beEqualTo("explicit");
      should(
        context.destination.channelInterpretation,
        "context.destination.channelInterpretation"
      ).beEqualTo("speakers");
      task.done();
    }
  );

  // Test constructor throws an error if the required members of the
  // dictionary are not given.
  audit.define(
    { label: "options-1", description: "Required options" },
    (task, should) => {
      let context2;

      // No args should throw
      should(() => {
        new OfflineAudioContext();
      }, "new OfflineAudioContext()").throw(TypeError);

      // Empty OfflineAudioContextOptions should throw
      should(() => {
        new OfflineAudioContext({});
      }, "new OfflineAudioContext({})").throw(TypeError);

      let options = { length: 42 };
      // sampleRate is required.
      should(() => {
        new OfflineAudioContext(options);
      }, "new OfflineAudioContext(" + JSON.stringify(options) + ")").throw(
        TypeError
      );

      options = { sampleRate: 12345 };
      // length is required.
      should(() => {
        new OfflineAudioContext(options);
      }, "new OfflineAudioContext(" + JSON.stringify(options) + ")").throw(
        TypeError
      );

      // Valid constructor.  Verify that the resulting context has the
      // correct values.
      options = { length: 42, sampleRate: 12345 };
      should(() => {
        context2 = new OfflineAudioContext(options);
      }, "c2 = new OfflineAudioContext(" + JSON.stringify(options) + ")").notThrow();
      should(
        context2.destination.channelCount,
        "c2.destination.channelCount"
      ).beEqualTo(1);
      should(context2.length, "c2.length").beEqualTo(options.length);
      should(context2.sampleRate, "c2.sampleRate").beEqualTo(
        options.sampleRate
      );
      should(
        context2.destination.channelCountMode,
        "c2.destination.channelCountMode"
      ).beEqualTo("explicit");
      should(
        context2.destination.channelInterpretation,
        "c2.destination.channelInterpretation"
      ).beEqualTo("speakers");

      task.done();
    }
  );

  // Constructor should throw errors for invalid values specified by
  // OfflineAudioContextOptions.
  audit.define(
    { label: "options-2", description: "Invalid options" },
    (task, should) => {
      let options = { length: 42, sampleRate: 8000, numberOfChannels: 33 };

      // channelCount too large.
      should(() => {
        new OfflineAudioContext(options);
      }, "new OfflineAudioContext(" + JSON.stringify(options) + ")").throw(
        RangeError,
        "NotSupportedError"
      );

      // length cannot be 0
      options = { length: 0, sampleRate: 8000 };
      should(() => {
        new OfflineAudioContext(options);
      }, "new OfflineAudioContext(" + JSON.stringify(options) + ")").throw(
        RangeError,
        "NotSupportedError"
      );

      // sampleRate outside valid range
      options = { length: 1, sampleRate: 1 };
      should(() => {
        new OfflineAudioContext(options);
      }, "new OfflineAudioContext(" + JSON.stringify(options) + ")").throw(
        RangeError,
        "NotSupportedError"
      );

      task.done();
    }
  );

  audit.define(
    { label: "options-3", description: "Valid options" },
    (task, should) => {
      let context;
      let options = {
        length: 1,
        sampleRate: 8000,
      };

      // Verify context with valid constructor has the correct values.
      should(() => {
        context = new OfflineAudioContext(options);
      }, "c = new OfflineAudioContext" + JSON.stringify(options) + ")").notThrow();
      should(context.length, "c.length").beEqualTo(options.length);
      should(context.sampleRate, "c.sampleRate").beEqualTo(options.sampleRate);
      should(
        context.destination.channelCount,
        "c.destination.channelCount"
      ).beEqualTo(1);
      should(
        context.destination.channelCountMode,
        "c.destination.channelCountMode"
      ).beEqualTo("explicit");
      should(
        context.destination.channelInterpretation,
        "c.destination.channelCountMode"
      ).beEqualTo("speakers");

      options.numberOfChannels = 7;
      should(() => {
        context = new OfflineAudioContext(options);
      }, "c = new OfflineAudioContext" + JSON.stringify(options) + ")").notThrow();
      should(
        context.destination.channelCount,
        "c.destination.channelCount"
      ).beEqualTo(options.numberOfChannels);

      task.done();
    }
  );

  globalThis.add_completion_callback((test) => {
    console.log("complate!");
    // console.log(globalThis.document.documentElement.outerHTML);
    // fs.writeFileSync("./log.html", globalThis.document.documentElement.outerHTML);
    const formatRet = retFormatter(test);
    // console.log(formatRet);
    for (let i = 0; i < formatRet.length; i++) {
      if (formatRet[i]["status"] != "Pass") {
        // console.log(
        //   `{\n\tname:${formatRet[i]["name"]}\n\tmessage:${formatRet[i]["message"]}\n}`
        // );
        console.log(formatRet[i]);
      }
    }
    // console.log(retFormatter(test));
  });

  audit.run();
}

module.exports = { wptOfflineAudioContextTest };
