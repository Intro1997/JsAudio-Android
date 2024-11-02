const { wptGainNodeTest0 } = require("js_tests/wpt_test/gainnode/ctor-gain.js");
const {
  wptGainNodeTest1,
} = require("js_tests/wpt_test/gainnode/gain-basic.js");
const { wptGainNodeTest2 } = require("js_tests/wpt_test/gainnode/gain.js");

async function GainNodeTest() {
  await wptGainNodeTest0();
  return wptGainNodeTest1();
}

module.exports = {
  GainNodeTest,
};
