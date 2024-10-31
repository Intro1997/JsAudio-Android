const testName =
  "wptAudioBufferTest<crashtests/copyFromChannel-bufferOffset-1.js>";

function wptAudioBufferTest4() {
  const a = new AudioBuffer({ length: 0x51986, sampleRate: 44100 });
  const b = new Float32Array(0x10);
  a.getChannelData(0); // to avoid zero data optimization
  a.copyFromChannel(b, 0, 0x1523c7cc);

  console.log(`✅ ${testName} passed!`);
}

module.exports = { wptAudioBufferTest4 };
