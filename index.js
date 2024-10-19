const {
  OfflineAudioContextTest,
} = require("js_tests/offline_audio_context_test.js");
const { OscillatorTest } = require("js_tests/oscillator_test.js");

async function AllTest() {
  await OscillatorTest();
  await OfflineAudioContextTest();
}

AllTest();
