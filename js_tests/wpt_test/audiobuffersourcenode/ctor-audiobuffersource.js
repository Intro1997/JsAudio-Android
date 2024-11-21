const testName = "wptAudioBufferSourceNodeTest0<ctor-audiobuffersource.js>";
const {
  waitUntilTestComplete,
  requireReload,
} = require("js_tests/wpt_test/helper/polyfile.js");

function wptAudioBufferSourceNodeTest0() {
  const { getDomException } = require("js_tests/wpt_test/helper/polyfile.js");
  globalThis.self = globalThis.window = {};
  requireReload("js_tests/wpt_test/helper/testharness.js");
  requireReload("js_tests/wpt_test/helper/testharnessreport.js");
  requireReload("js_tests/wpt_test/helper/audit.js");
  const {
    initializeContext,
    testInvalidConstructor,
    testDefaultConstructor,
    testDefaultAttributes,
  } = require("js_tests/wpt_test/helper/audionodeoptions.js");

  Object.assign(globalThis, globalThis.self);
  window["AudioBufferSourceNode"] = AudioBufferSourceNode;
  globalThis.DOMException = getDomException();
  let context;

  let audit = Audit.createTaskRunner();

  audit.define("initialize", (task, should) => {
    context = initializeContext(should);
    task.done();
  });

  audit.define("invalid constructor", (task, should) => {
    testInvalidConstructor(should, "AudioBufferSourceNode", context);
    task.done();
  });

  audit.define("default constructor", (task, should) => {
    let prefix = "node0";
    let node = testDefaultConstructor(
      should,
      "AudioBufferSourceNode",
      context,
      {
        prefix: prefix,
        numberOfInputs: 0,
        numberOfOutputs: 1,
        channelCount: 2,
        channelCountMode: "max",
        channelInterpretation: "speakers",
      }
    );

    testDefaultAttributes(should, node, prefix, [
      { name: "buffer", value: null },
      { name: "detune", value: 0 },
      { name: "loop", value: false },
      { name: "loopEnd", value: 0.0 },
      { name: "loopStart", value: 0.0 },
      { name: "playbackRate", value: 1.0 },
    ]);

    task.done();
  });

  audit.define("nullable buffer", (task, should) => {
    let node;
    let options = { buffer: null };

    should(() => {
      node = new AudioBufferSourceNode(context, options);
    }, "node1 = new AudioBufferSourceNode(c, " + JSON.stringify(options)).notThrow();

    should(node.buffer, "node1.buffer").beEqualTo(null);

    task.done();
  });

  audit.define("constructor options", (task, should) => {
    let node;
    let buffer = context.createBuffer(2, 1000, context.sampleRate);

    let options = {
      buffer: buffer,
      detune: 0.5,
      loop: true,
      loopEnd: buffer.length / 2 / context.sampleRate,
      loopStart: 5 / context.sampleRate,
      playbackRate: 0.75,
    };

    let message =
      "node = new AudioBufferSourceNode(c, " + JSON.stringify(options) + ")";

    should(() => {
      node = new AudioBufferSourceNode(context, options);
    }, message).notThrow();

    // Use the factory method to create an equivalent node and compare the
    // results from the constructor against this node.
    let factoryNode = context.createBufferSource();
    factoryNode.buffer = options.buffer;
    factoryNode.detune.value = options.detune;
    factoryNode.loop = options.loop;
    factoryNode.loopEnd = options.loopEnd;
    factoryNode.loopStart = options.loopStart;
    factoryNode.playbackRate.value = options.playbackRate;

    should(node.buffer === buffer, "node2.buffer === buffer").beEqualTo(true);
    should(node.detune.value, "node2.detune.value").beEqualTo(
      factoryNode.detune.value
    );
    should(node.loop, "node2.loop").beEqualTo(factoryNode.loop);
    should(node.loopEnd, "node2.loopEnd").beEqualTo(factoryNode.loopEnd);
    should(node.loopStart, "node2.loopStart").beEqualTo(factoryNode.loopStart);
    should(node.playbackRate.value, "node2.playbackRate.value").beEqualTo(
      factoryNode.playbackRate.value
    );

    task.done();
  });

  audit.run();
  return waitUntilTestComplete(globalThis, testName);
}

module.exports = { wptAudioBufferSourceNodeTest0 };
