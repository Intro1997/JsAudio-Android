const {
  OfflineAudioContextTest,
} = require("js_tests/offline_audio_context_test.js");
const { OscillatorTest } = require("js_tests/oscillator_test.js");
const { AudioBufferTest } = require("js_tests/audio_buffer_test.js");

async function AllTest() {
  await AudioBufferTest();
  await OscillatorTest();
  await OfflineAudioContextTest();
}

AllTest();
