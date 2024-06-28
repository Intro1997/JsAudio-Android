// setInterval(() => {
//   native_log("hello from interval");
// }, 1000);
// native_log("log check: hello from native log");

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

// log_test();
// schedual_run_test();
// set_timeout_test();
audio_test();
