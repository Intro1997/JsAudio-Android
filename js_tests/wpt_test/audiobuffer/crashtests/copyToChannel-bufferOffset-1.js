const testName =
  "wptAudioBufferTest5<crashtests/copyToChannel-bufferOffset-1.js>";

function wptAudioBufferTest5() {
  const a = new AudioBuffer({ length: 0x10, sampleRate: 44100 });
  const b = new Float32Array(0x51986);
  a.copyToChannel(b, 0, 0x40004000);

  console.log(`✅ ${testName} passed!`);
}

module.exports = { wptAudioBufferTest5 };
