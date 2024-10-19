const {
  wptOfflineAudioContextTest0,
} = require("js_tests/wpt_test/offline_audio_context/ctor-offlineaudiocontext.js");
const {
  wptOfflineAudioContextTest1,
} = require("js_tests/wpt_test/offline_audio_context/current-time-block-size.js");

function BasicTest() {
  let options = {
    numberOfChannels: 2,
    length: 44100 * 40,
    sampleRate: 44100.123,
  };

  let offlineAudioCtx = new OfflineAudioContext(options);
  offlineAudioCtx.startRendering().then((event) => {
    console.log(
      "event.renderedBuffer.getChannelData(0) = ",
      event.renderedBuffer.getChannelData(0)
    );
    console.log(
      "event.renderedBuffer.getChannelData(1) = ",
      event.renderedBuffer.getChannelData(1)
    );
  });
}
async function OfflineAudioContextTest() {
  await wptOfflineAudioContextTest0();
  return wptOfflineAudioContextTest1();
  // BasicTest()
}

module.exports = { OfflineAudioContextTest };
