const {
  wptOscillatorTest0,
} = require("js_tests/wpt_test/oscillator/ctor-oscillator.js");

function tracePrototypeChainOf(object) {
  var proto = object.constructor.prototype;
  var result = "";

  while (proto) {
    result += proto.constructor.name + ".prototype -> ";
    proto = Object.getPrototypeOf(proto);
  }

  result += "null";
  return result;
}

function CheckAudioParam(name, audio_param, modify_value) {
  console.log(name, ": ");
  console.log("audio_param.value = ", audio_param.value);
  audio_param.value = modify_value;
  console.log("\tafter audio_param.value set = ", audio_param.value);
  console.log("audio_param.automationRate = ", audio_param.automationRate);
  console.log("audio_param.defaultValue = ", audio_param.defaultValue);
  console.log("audio_param.minValue = ", audio_param.minValue);
  console.log("audio_param.maxValue = ", audio_param.maxValue);
}

function OscillatorWaveTypeTest() {
  const audioCtx = new AudioContext();
  const oscillatorNode = audioCtx.createOscillator();
  oscillatorNode.type = "sine";
  console.log("oscillatorNode.type = ", oscillatorNode.type);
  oscillatorNode.connect(audioCtx.destination);

  setTimeout(() => {
    oscillatorNode.type = "sawtooth";
  }, 1000);
  setTimeout(() => {
    oscillatorNode.type = "square";
  }, 2000);
  setTimeout(() => {
    oscillatorNode.type = "triangle";
  }, 3000);
}

async function OscillatorTest() {
  return wptOscillatorTest0();
}

module.exports = { OscillatorTest };
