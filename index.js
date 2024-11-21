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
  await AudioBufferTest();
  await OscillatorTest();
  await OfflineAudioContextTest();
  await DestinationNodeTest();
  await GainNodeTest();
  await DelayNodeTest();
  await AudioBufferSourceNodeTest();
}

async function GetTuneAudioBuffer(tunes, numberOfChannels, ctx) {
  const audioBuffer = new AudioBuffer({
    numberOfChannels: numberOfChannels,
    length: ctx.sampleRate * tunes.length,
    sampleRate: ctx.sampleRate,
  });
  for (let i = 0; i < tunes.length; i++) {
    console.log("flag01");
    const offline_audio_ctx = new OfflineAudioContext(
      numberOfChannels,
      ctx.sampleRate,
      ctx.sampleRate
    );
    console.log("flag02");
    const osc = new OscillatorNode(offline_audio_ctx, { frequency: tunes[i] });
    console.log("flag03");
    osc.connect(offline_audio_ctx.destination);
    console.log("flag04");
    osc.start();
    console.log("flag05");
    let buffer = await offline_audio_ctx.startRendering();
    audioBuffer.copyToChannel(buffer.getChannelData(0), 0, i * ctx.sampleRate);
    audioBuffer.copyToChannel(buffer.getChannelData(1), 1, i * ctx.sampleRate);
  }
  return audioBuffer;
}

// const tune = [261.625, 293.664, 329.628, 349.228, 391.995, 440, 493.883];

// let ctx = new AudioContext();

async function Test(loopStart, loopEnd, playbackRate) {
  // let ctx = new AudioContext();

  let audioBuffer = await GetTuneAudioBuffer(tune, 2, ctx);

  let audioBufferSourceNode = ctx.createBufferSource();
  audioBufferSourceNode.buffer = audioBuffer;
  audioBufferSourceNode.loop = true;
  audioBufferSourceNode.loopStart = loopStart;
  audioBufferSourceNode.loopEnd = loopEnd;
  audioBufferSourceNode.playbackRate = playbackRate;

  // let audioBufferSourceNode = new AudioBufferSourceNode(ctx, {
  //   buffer: audioBuffer,
  //   loop: true,
  //   loopStart: loopStart,
  //   loopEnd: loopEnd,
  //   playbackRate: playbackRate,
  // });
  console.log("before start");
  audioBufferSourceNode.start(0, 0, 20);
  console.log("before connect");
  audioBufferSourceNode.connect(ctx.destination);

  setTimeout(() => {
    console.log("before stop");
    audioBufferSourceNode.stop();
  }, 14000);
}

AllTest();
// Test(3, 7, 1.0);
