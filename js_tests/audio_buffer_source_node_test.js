const {
  wptAudioBufferSourceNodeTest0,
} = require("js_tests/wpt_test/audiobuffersourcenode/ctor-audiobuffersource.js");
const {
  wptAudioBufferSourceNodeTest1,
} = require("js_tests/wpt_test/audiobuffersourcenode/audiobuffersource-basic.js");
const {
  wptAudioBufferSourceNodeTest2,
} = require("js_tests/wpt_test/audiobuffersourcenode/audiobuffersource-channels.js");
const {
  wptAudioBufferSourceNodeTest3,
} = require("js_tests/wpt_test/audiobuffersourcenode/audiobuffersource-duration-loop.js");
const {
  wptAudioBufferSourceNodeTest4,
} = require("js_tests/wpt_test/audiobuffersourcenode/audiobuffersource-grain.js");
const {
  wptAudioBufferSourceNodeTest5,
} = require("js_tests/wpt_test/audiobuffersourcenode/audiobuffersource-null.js");
const {
  wptAudioBufferSourceNodeTest6,
} = require("js_tests/wpt_test/audiobuffersourcenode/audiobuffersource-one-sample-loop.js");
const {
  wptAudioBufferSourceNodeTest7,
} = require("js_tests/wpt_test/audiobuffersourcenode/audiobuffersource-start.js");
const {
  wptAudioBufferSourceNodeTest8,
} = require("js_tests/wpt_test/audiobuffersourcenode/audiosource-time-limits.js");
const {
  wptAudioBufferSourceNodeTest9,
} = require("js_tests/wpt_test/audiobuffersourcenode/note-grain-on-play.js");
const {
  wptAudioBufferSourceNodeTest10,
} = require("js_tests/wpt_test/audiobuffersourcenode/note-grain-on-timing.js");
const {
  wptAudioBufferSourceNodeTest11,
} = require("js_tests/wpt_test/audiobuffersourcenode/sample-accurate-scheduling.js");

async function AudioBufferSourceNodeTest() {
  await wptAudioBufferSourceNodeTest0();
  await wptAudioBufferSourceNodeTest1();
  await wptAudioBufferSourceNodeTest2();
  await wptAudioBufferSourceNodeTest3();
  await wptAudioBufferSourceNodeTest4();
  await wptAudioBufferSourceNodeTest5();
  await wptAudioBufferSourceNodeTest6();
  await wptAudioBufferSourceNodeTest7();
  await wptAudioBufferSourceNodeTest8();
  await wptAudioBufferSourceNodeTest9();
  await wptAudioBufferSourceNodeTest10();
  return wptAudioBufferSourceNodeTest11();
}

module.exports = {
  AudioBufferSourceNodeTest,
};
