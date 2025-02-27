/**
 * Adaptive Changes：
 * 1. Comment last four test
 *    - we do not support modify channelCount of destiantion node
 */

const testName = "wptDestinationTest0<destination.js>";
const {
  waitUntilTestComplete,
  requireReload,
} = require("js_tests/wpt_test/helper/polyfile.js");

function wptDestinationTest0() {
  const { getDomException } = require("js_tests/wpt_test/helper/polyfile.js");
  globalThis.self = globalThis.window = {};

  requireReload("js_tests/wpt_test/helper/testharness.js");
  requireReload("js_tests/wpt_test/helper/testharnessreport.js");

  Object.assign(globalThis, globalThis.self);
  globalThis.DOMException = getDomException();

  function assert_doesnt_throw(f, desc) {
    try {
      f();
    } catch (e) {
      assert_true(false, desc);
      return;
    }
    assert_true(true, desc);
  }

  test(function () {
    var ac = new AudioContext();
    assert_equals(
      ac.destination.channelCount,
      2,
      "A DestinationNode should have two channels by default"
    );
    assert_greater_than_equal(
      ac.destination.maxChannelCount,
      2,
      "maxChannelCount should be >= 2"
    );
    // assert_throws_dom(
    //   "IndexSizeError",
    //   function () {
    //     ac.destination.channelCount = ac.destination.maxChannelCount + 1;
    //   },
    //   `Setting the channelCount to something greater than
    //           the maxChannelCount should throw IndexSizeError`
    // );
    // assert_throws_dom(
    //   "NotSupportedError",
    //   function () {
    //     ac.destination.channelCount = 0;
    //   },
    //   "Setting the channelCount to 0 should throw NotSupportedError"
    // );
    // assert_doesnt_throw(function () {
    //   ac.destination.channelCount = ac.destination.maxChannelCount;
    // }, "Setting the channelCount to maxChannelCount should not throw");
    // assert_doesnt_throw(function () {
    //   ac.destination.channelCount = 1;
    // }, "Setting the channelCount to 1 should not throw");
  });

  return waitUntilTestComplete(globalThis, testName);
}

module.exports = { wptDestinationTest0 };
