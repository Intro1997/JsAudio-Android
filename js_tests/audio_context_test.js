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
  if (BaseAudioContext !== undefined) {
    error_msg.push(
      `Error! BaseAudioContext is not undefined, you cannot export BaseAudioContext`
    );
  }

  if (AudioContext === undefined) {
    error_msg.push(
      `Error! AudioContext is undefined, you should export AudioContext!`
    );
  }

  if (AudioDestinationNode !== undefined) {
    error_msg.push(
      `Error! AudioDestinationNode is not undefined, you cannot export AudioDestinationNode`
    );
  }
}

function PrototypeTest() {
  const audioCtx = new AudioContext();
  tracePrototypeChainOf(audioCtx);
  tracePrototypeChainOf(audioCtx.destination);
}

function AudioContextTest() {
  ExportsTest();
  PrototypeTest();
  error_msg.forEach((msg) => {
    console.log(msg);
  });
}

module.exports = { AudioContextTest };
