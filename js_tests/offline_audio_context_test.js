const {
  wptOfflineAudioContextTest,
} = require("js_tests/wpt_test/offline_audio_context/ctor-offlineaudiocontext.js");

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
  // offlineAudioCtx = new OfflineAudioContext(2, 44100 * 40, 44100.123);
}
function OfflineAudioContextTest() {
  wptOfflineAudioContextTest();
  // BasicTest()

  // let context2;
  // let options = { length: 42, sampleRate: 12345 };
  // context2 = new OfflineAudioContext(options);
  // console.log("context2.destination.channelCount = ", context2.destination.channelCount);
}

module.exports = { OfflineAudioContextTest };
