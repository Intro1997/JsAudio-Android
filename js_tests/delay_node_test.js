const {
  wptDelayNodeTest0,
} = require("js_tests/wpt_test/delaynode/ctor-delay.js");
const {
  wptDelayNodeTest1,
} = require("js_tests/wpt_test/delaynode/delaynode-max-default-delay.js");
const {
  wptDelayNodeTest2,
} = require("js_tests/wpt_test/delaynode/delaynode-max-nondefault-delay.js");
const {
  wptDelayNodeTest3,
} = require("js_tests/wpt_test/delaynode/delaynode-maxdelay.js");
const {
  wptDelayNodeTest4,
} = require("js_tests/wpt_test/delaynode/delaynode-maxdelaylimit.js");
const {
  wptDelayNodeTest5,
} = require("js_tests/wpt_test/delaynode/delaynode.js");

async function DelayNodeTest() {
  await wptDelayNodeTest0();
  await wptDelayNodeTest1();
  await wptDelayNodeTest2();
  await wptDelayNodeTest3();
  await wptDelayNodeTest4();
  return wptDelayNodeTest5();
}

module.exports = { DelayNodeTest };
