const testName = "wptDelayNodeTest0<ctor-delay.js>";
const {
  waitUntilTestComplete,
  requireReload,
} = require("js_tests/wpt_test/helper/polyfile.js");

async function wptDelayNodeTest0() {
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
  globalThis.DOMException = getDomException();
  window["DelayNode"] = DelayNode;

  let context;

  let audit = Audit.createTaskRunner();

  audit.define("initialize", (task, should) => {
    context = initializeContext(should);
    task.done();
  });

  audit.define("invalid constructor", (task, should) => {
    testInvalidConstructor(should, "DelayNode", context);
    task.done();
  });

  audit.define("default constructor", (task, should) => {
    let prefix = "node0";
    let node = testDefaultConstructor(should, "DelayNode", context, {
      prefix: prefix,
      numberOfInputs: 1,
      numberOfOutputs: 1,
      channelCount: 2,
      channelCountMode: "max",
      channelInterpretation: "speakers",
    });

    testDefaultAttributes(should, node, prefix, [
      { name: "delayTime", value: 0 },
    ]);

    task.done();
  });

  audit.define("test AudioNodeOptions", (task, should) => {
    testAudioNodeOptions(should, context, "DelayNode");
    task.done();
  });

  audit.define("constructor options", (task, should) => {
    let node;
    let options = {
      delayTime: 0.5,
      maxDelayTime: 1.5,
    };

    should(() => {
      node = new DelayNode(context, options);
    }, "node1 = new DelayNode(c, " + JSON.stringify(options) + ")").notThrow();

    should(node.delayTime.value, "node1.delayTime.value").beEqualTo(
      options.delayTime
    );
    should(node.delayTime.maxValue, "node1.delayTime.maxValue").beEqualTo(
      options.maxDelayTime
    );

    task.done();
  });

  audit.run();

  return waitUntilTestComplete(globalThis, testName);
}

module.exports = { wptDelayNodeTest0 };
