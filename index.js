const {
  OfflineAudioContextTest,
} = require("js_tests/offline_audio_context_test.js");
const { OscillatorTest } = require("js_tests/oscillator_test.js");
const { AudioBufferTest } = require("js_tests/audio_buffer_test.js");
const { DestinationNodeTest } = require("js_tests/destination_node_test.js");
const { GainNodeTest } = require("js_tests/gain_node_test.js");
const { DelayNodeTest } = require("js_tests/delay_node_test.js");
const {
  AudioBufferSourceNodeTest,
} = require("js_tests/audio_buffer_source_node_test.js");

async function AllTest() {
  const task_list = [
    AudioBufferTest,
    OscillatorTest,
    OfflineAudioContextTest,
    DestinationNodeTest,
    GainNodeTest,
    DelayNodeTest,
    AudioBufferSourceNodeTest,
  ];

  for (let i = 0; i < task_list.length; i++) {
    await task_list[i]();
    // do gc() to clear nodejs async queue
    global.gc();
  }
}

AllTest();
