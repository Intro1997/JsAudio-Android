function AudioBufferTest() {
  const audioCtx = new AudioContext();

  const numberOfChannel = 2;
  const bufferLength = 10;
  const sampleRate = 100;

  const audioBuffer = audioCtx.createBuffer(
    numberOfChannel,
    bufferLength,
    sampleRate
  );

  console.log(`audioBuffer.sampleRate = ${audioBuffer.sampleRate}`);
  console.log(`audioBuffer.length = ${audioBuffer.length}`);
  console.log(`audioBuffer.duration = ${audioBuffer.duration}`);
  console.log(
    `audioBuffer.numberOfChannels  = ${audioBuffer.numberOfChannels}`
  );

  const sourceDataArray0 = new Float32Array(bufferLength);
  const sourceDataArray1 = new Float32Array(bufferLength);
  for (let i = 0; i < sourceDataArray0.length; i++) {
    sourceDataArray0[i] = 42 + i;
    sourceDataArray1[i] = 42 - i;
  }

  console.log(`sourceDataArray0 = `, sourceDataArray0);
  console.log(`sourceDataArray1 = `, sourceDataArray1);

  audioBuffer.copyToChannel(sourceDataArray0, 0, 0);
  audioBuffer.copyToChannel(sourceDataArray1, 1, 0);

  console.log(
    "audioBuffer.getChannelData(0) = ",
    audioBuffer.getChannelData(0)
  );
  console.log(
    "audioBuffer.getChannelData(1) = ",
    audioBuffer.getChannelData(1)
  );

  const channelDestArray = new Float32Array(bufferLength);
  const startIdx = 0;
  audioBuffer.copyFromChannel(channelDestArray, 0, startIdx);
  console.log(`copy from channel 0: channelDestArray = ${channelDestArray}`);
  audioBuffer.copyFromChannel(channelDestArray, 1, startIdx);
  console.log(`copy from channel 1: channelDestArray = ${channelDestArray}`);
}

module.exports = { AudioBufferTest };
