const total_error_msg = [];
const prototypeChainValue = {
  AudioContext:
    "AudioContext.prototype -> BaseAudioContext.prototype -> Object.prototype -> null",
  AudioDestinationNode:
    "AudioDestinationNode.prototype -> AudioNode.prototype -> Object.prototype -> null",
  GetError: (str_type) => {
    return `${str_type} prototype failed!!`;
  },
};

function isEqual(val, expect_val, err_msg) {
  if (val !== expect_val) {
    err_msg += `\n\tShould be <${expect_val}> but get <${val}>`;
    total_error_msg.push(err_msg);
  }
}

function isValid(val, err_msg) {
  if (!val) {
    total_error_msg.push(err_msg);
  }
}
function isTypeOf(type, expect_type, err_msg) {
  isEqual(typeof type, typeof expect_type, err_msg);
}

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

function ExportsTest() {
  if (typeof BaseAudioContext !== "undefined") {
    total_error_msg.push(
      `Error! BaseAudioContext is not undefined, you cannot export BaseAudioContext`
    );
  }

  if (typeof AudioContext === "undefined") {
    total_error_msg.push(
      `Error! AudioContext is undefined, you should export AudioContext!`
    );
  }

  if (typeof AudioDestinationNode !== "undefined") {
    total_error_msg.push(
      `Error! AudioDestinationNode is not undefined, you cannot export AudioDestinationNode`
    );
  }
}

class PrototypeTest {
  static TotalPrototypeTest() {
    PrototypeTest.PrototypePropertyTest();
    PrototypeTest.PrototypeChainTest();
  }
  static PrototypeChainTest() {
    const audioCtx = new AudioContext();
    isEqual(
      tracePrototypeChainOf(audioCtx),
      prototypeChainValue.AudioContext,
      prototypeChainValue.GetError("AudioContext")
    );
    isEqual(
      tracePrototypeChainOf(audioCtx.destination),
      prototypeChainValue.AudioDestinationNode,
      prototypeChainValue.GetError("AudioDestinationNode")
    );
  }

  static PrototypePropertyTest() {
    const audioCtx = new AudioContext();
    PrototypeTest.BaseAudioContextPrototypePropertyTest(audioCtx);
    PrototypeTest.AudioContextPrototypePropertyTest(audioCtx);
  }

  static BaseAudioContextPrototypePropertyTest(baseAudioCtx) {
    PropertyValid(baseAudioCtx);
    PropertyType(baseAudioCtx);

    function PropertyValid(baseAudioCtx) {
      isValid(
        baseAudioCtx.destination,
        "cannot find valid destination property on BaseAudioContext"
      );
      isValid(
        baseAudioCtx.sampleRate,
        "cannot find valid sampleRate property on BaseAudioContext"
      );
      isValid(
        baseAudioCtx.currentTime,
        "cannot find valid currentTime property on BaseAudioContext"
      );
      isValid(
        baseAudioCtx.createBuffer,
        "cannot find valid createBuffer property on BaseAudioContext"
      );
      isValid(
        baseAudioCtx.createBufferSource,
        "cannot find valid createBufferSource property on BaseAudioContext"
      );
      isValid(
        baseAudioCtx.createDelay,
        "cannot find valid createDelay property on BaseAudioContext"
      );
      isValid(
        baseAudioCtx.createGain,
        "cannot find valid createGain property on BaseAudioContext"
      );
      isValid(
        baseAudioCtx.createOscillator,
        "cannot find valid createOscillator property on BaseAudioContext"
      );
    }
    function PropertyType(baseAudioCtx) {
      isTypeOf(
        baseAudioCtx.destination,
        {},
        "BaseAudioContext.destination type error"
      );
      isTypeOf(
        baseAudioCtx.sampleRate,
        0,
        "BaseAudioContext.sampleRate type error"
      );
      isTypeOf(
        baseAudioCtx.currentTime,
        0,
        "BaseAudioContext.currentTime type error"
      );

      isTypeOf(
        baseAudioCtx.createBuffer,
        () => {},
        "BaseAudioContext.createBuffer type error"
      );
      isTypeOf(
        baseAudioCtx.createBufferSource,
        () => {},
        "BaseAudioContext.createBufferSource type error"
      );
      isTypeOf(
        baseAudioCtx.createDelay,
        () => {},
        "BaseAudioContext.createDelay type error"
      );
      isTypeOf(
        baseAudioCtx.createGain,
        () => {},
        "BaseAudioContext.createGain type error"
      );
      isTypeOf(
        baseAudioCtx.createOscillator,
        () => {},
        "BaseAudioContext.createOscillator type error"
      );
    }
  }

  static AudioContextPrototypePropertyTest(audioCtx) {
    PropertyValid(audioCtx);
    PropertyType(audioCtx);

    function PropertyValid(audioCtx) {
      isValid(audioCtx.constructor, "AudioContext.constructor is valid!");
      isValid(audioCtx.resume, "AudioContext.resume is valid!");
      isValid(audioCtx.suspend, "AudioContext.suspend is valid!");
      isValid(audioCtx.close, "AudioContext.close is valid!");
    }
    function PropertyType(audioCtx) {
      isTypeOf(
        audioCtx.constructor,
        () => {},
        "AudioContext.constructor type error"
      );
      isTypeOf(audioCtx.resume, () => {}, "AudioContext.resume type error");
      isTypeOf(audioCtx.suspend, () => {}, "AudioContext.suspend type error");
      isTypeOf(audioCtx.close, () => {}, "AudioContext.close type error");
    }
  }
}

function PrintErrorIfHave() {
  if (total_error_msg.length) {
    console.error("AudioContext Test NOT PASS!");
    total_error_msg.forEach((element) => {
      console.error("\t", element);
    });
  } else {
    console.log("AudioContext Test PASS!");
  }
}

function AudioContextTest() {
  ExportsTest();
  PrototypeTest.TotalPrototypeTest();

  PrintErrorIfHave();
}

module.exports = { AudioContextTest };
