function OscillatorTest() {
  console.log("hi in oscillator test");
}

function log_test() {
  const a = { b: 1, c: 2 };
  console.log(a);
  console.info("hello");
  console.warn(`a.b = ${a.b}`);
  console.error(123123);
  console.log("my name is ", a.c);
}

function set_interval_test() {
  setInterval(() => {
    console.log("hello from interval");
  }, 1000);
}

function set_timeout_test() {
  console.log("before timeout run");
  setTimeout(() => {
    // TODO(FIXME): when app pause, timeout schedule will not stop
    // it will still print message even if app is stop
    console.log("Dangdang! TIMEOUT!");
  }, 5000);
}

function audio_test() {
  const audioCtx = new AudioContext();
  console.log(tracePrototypeChainOf(audioCtx));
  console.log(tracePrototypeChainOf(audioCtx.destination));
  console.log(
    `audioCtx.destination.numberOfInputs = ${audioCtx.destination.numberOfInputs}`
  );
  console.log(
    `audioCtx.destination.numberOfOutputs = ${audioCtx.destination.numberOfOutputs}`
  );
  console.log(
    `audioCtx.destination.channelCount = ${audioCtx.destination.channelCount}`
  );
  console.log(
    `audioCtx.destination.destinationSpecified = ${audioCtx.destination.destinationSpecified}`
  );
  audioCtx.destination.destinationSpecified = 424242;
  console.log(
    `audioCtx.destination.destinationSpecified [after modify] = ${audioCtx.destination.destinationSpecified}`
  );
}

function tracePrototypeChainOf(object) {
  var proto = object.constructor.prototype;
  var result = "";

  while (proto) {
    result += " -> " + proto.constructor.name + ".prototype";
    proto = Object.getPrototypeOf(proto);
  }

  result += " -> null";
  return result;
}

function ExportsTest() {
  let error_msg = [];
  if (typeof BaseAudioContext !== "undefined") {
    error_msg.push(
      `Error! BaseAudioContext is not undefined, you cannot export BaseAudioContext`
    );
  }

  if (typeof AudioContext === "undefined") {
    error_msg.push(
      `Error! AudioContext is undefined, you should export AudioContext!`
    );
  }

  if (typeof AudioDestinationNode !== "undefined") {
    error_msg.push(
      `Error! AudioDestinationNode is not undefined, you cannot export AudioDestinationNode`
    );
  }
  error_msg.forEach((msg) => {
    console.log(msg);
  });
}

function PrototypeTest() {
  const audioCtx = new AudioContext();
  console.log(tracePrototypeChainOf(audioCtx));
  console.log(tracePrototypeChainOf(audioCtx.destination));
}

function AudioNodePropertyTest(audioNode) {
  console.log("audioNode.numberOfInputs = ", audioNode.numberOfInputs);
  console.log("audioNode.numberOfOutputs = ", audioNode.numberOfOutputs);
  console.log("audioNode.channelCount = ", audioNode.channelCount);
}

function AudioDestinationNodePropertyTest(destinationNode) {
  AudioNodePropertyTest(destinationNode);
  console.log(
    "destinationNode.destinationSpecified = ",
    destinationNode.destinationSpecified
  );
}

function BaseAudioContextPropertyTest(baseAudioContext) {
  console.log("audioCtx.destination = ", baseAudioContext.destination);
  console.log("audioCtx.sampleRate = ", baseAudioContext.sampleRate);
  console.log("audioCtx.currentTime = ", baseAudioContext.currentTime);

  AudioDestinationNodePropertyTest(baseAudioContext.destination);
}

function AudioContextPropertyTest(audioContext) {
  audioContext.resume();
  audioContext.suspend();
  audioContext.close();
}

function AudioContextTest() {
  ExportsTest();
  PrototypeTest();
  const audioCtx = new AudioContext();
  console.log(`audioCtx = ${audioCtx}`);
  BaseAudioContextPropertyTest(audioCtx);
  AudioContextPropertyTest(audioCtx);
}

module.exports = { OscillatorTest, AudioContextTest };
