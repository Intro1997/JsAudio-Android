const testName = "wptGainNodeTest0<ctor-gain.js>";
const {
  waitUntilTestComplete,
  requireReload,
} = require("js_tests/wpt_test/helper/polyfile.js");

function wptGainNodeTest0() {
  const { getDomException } = require("js_tests/wpt_test/helper/polyfile.js");
  globalThis.self = globalThis.window = {};
  requireReload("js_tests/wpt_test/helper/testharness.js");
  requireReload("js_tests/wpt_test/helper/testharnessreport.js");
  requireReload("js_tests/wpt_test/helper/audit.js");
  const {
    testAudioNodeOptions,
    initializeContext,
    testInvalidConstructor,
    testDefaultConstructor,
    testDefaultAttributes,
  } = require("js_tests/wpt_test/helper/audionodeoptions.js");

  Object.assign(globalThis, globalThis.self);
  window["GainNode"] = GainNode;
  console.log("GainNode = ", GainNode);
  globalThis.DOMException = getDomException();

  let context;

  let audit = Audit.createTaskRunner();

  audit.define("initialize", (task, should) => {
    context = initializeContext(should);
    task.done();
  });

  audit.define("invalid constructor", (task, should) => {
    testInvalidConstructor(should, "GainNode", context);
    task.done();
  });

  audit.define("default constructor", (task, should) => {
    let prefix = "node0";
    let node = testDefaultConstructor(should, "GainNode", context, {
      prefix: prefix,
      numberOfInputs: 1,
      numberOfOutputs: 1,
      channelCount: 2,
      channelCountMode: "max",
      channelInterpretation: "speakers",
    });

    testDefaultAttributes(should, node, prefix, [{ name: "gain", value: 1 }]);

    task.done();
  });

  audit.define("test AudioNodeOptions", (task, should) => {
    testAudioNodeOptions(should, context, "GainNode");
    task.done();
  });

  audit.define("constructor with options", (task, should) => {
    let node;
    let options = {
      gain: -2,
    };

    should(() => {
      node = new GainNode(context, options);
    }, "node1 = new GainNode(c, " + JSON.stringify(options) + ")").notThrow();
    should(node instanceof GainNode, "node1 instanceof GainNode").beEqualTo(
      true
    );

    should(node.gain.value, "node1.gain.value").beEqualTo(options.gain);

    should(node.channelCount, "node1.channelCount").beEqualTo(2);
    should(node.channelCountMode, "node1.channelCountMode").beEqualTo("max");
    should(node.channelInterpretation, "node1.channelInterpretation").beEqualTo(
      "speakers"
    );

    task.done();
  });

  audit.run();
  return waitUntilTestComplete(globalThis, testName);
}

module.exports = { wptGainNodeTest0 };