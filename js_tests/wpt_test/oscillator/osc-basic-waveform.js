/**
 * Adaptive Changes：
 * 1. comment Test 4 and Test 5
 *    - we do not support PeriodicWave now
 */

const testName = "wptOscillatorTest1<osc-basic-waveform.js>";
const {
  waitUntilTestComplete,
  requireReload,
} = require("js_tests/wpt_test/helper/polyfile.js");

function wptOscillatorTest1() {
  const { getDomException } = require("js_tests/wpt_test/helper/polyfile.js");
  globalThis.self = globalThis.window = {};

  requireReload("js_tests/wpt_test/helper/testharness.js");
  requireReload("js_tests/wpt_test/helper/testharnessreport.js");
  requireReload("js_tests/wpt_test/helper/audit.js");
  const {
    computeSNR,
    RENDER_QUANTUM_FRAMES,
  } = require("js_tests/wpt_test/helper/audit-util.js");

  Object.assign(globalThis, globalThis.self);
  window["OscillatorNode"] = OscillatorNode;
  globalThis.DOMException = getDomException();

  // Don't change the sample rate.  The tests below depend on this sample
  // rate to cover all the cases in Chrome's implementation.  But the tests
  // are general and apply to any browser.
  const sampleRate = 44100;

  // Only need a few samples for testing, so just use two renders.
  const durationFrames = 2 * RENDER_QUANTUM_FRAMES;

  let audit = Audit.createTaskRunner();

  // The following tests verify that the oscillator produces the same
  // results as the mathematical oscillators.  We choose sine wave and a
  // custom wave because we know they're bandlimited and won't change with
  // the frequency.
  //
  // The tests for 1 and 2 Hz are intended to test Chrome's interpolation
  // algorithm, but are still generally applicable to any browser.

  audit.define(
    { label: "Test 0", description: "Sine wave: 100 Hz" },
    async (task, should) => {
      let context = new OfflineAudioContext({
        length: durationFrames,
        sampleRate: sampleRate,
      });

      const freqHz = 100;

      let src = new OscillatorNode(context, {
        type: "sine",
        frequency: freqHz,
      });
      src.connect(context.destination);

      src.start();

      let renderedBuffer = await context.startRendering();
      checkResult(should, renderedBuffer, context, {
        freqHz: freqHz,
        a1: 0,
        b1: 1,
        prefix: "Sine",
        threshold: 1.8045e-5,
        snrThreshold: 112.5,
      });
      task.done();
    }
  );

  audit.define(
    { label: "Test 1", description: "Sine wave: -100 Hz" },
    async (task, should) => {
      let context = new OfflineAudioContext({
        length: durationFrames,
        sampleRate: sampleRate,
      });

      const freqHz = -100;

      let src = new OscillatorNode(context, {
        type: "sine",
        frequency: freqHz,
      });
      src.connect(context.destination);

      src.start();

      let renderedBuffer = await context.startRendering();
      checkResult(should, renderedBuffer, context, {
        freqHz: freqHz,
        a1: 0,
        b1: 1,
        prefix: "Sine",
        threshold: 1.8045e-5,
        snrThreshold: 112.67,
      });
      task.done();
    }
  );

  audit.define(
    { label: "Test 2", description: "Sine wave: 2 Hz" },
    async (task, should) => {
      let context = new OfflineAudioContext({
        length: durationFrames,
        sampleRate: sampleRate,
      });

      const freqHz = 2;

      let src = new OscillatorNode(context, {
        type: "sine",
        frequency: freqHz,
      });
      src.connect(context.destination);

      src.start();

      let renderedBuffer = await context.startRendering();
      checkResult(should, renderedBuffer, context, {
        freqHz: freqHz,
        a1: 0,
        b1: 1,
        prefix: "Sine",
        threshold: 1.4516e-7,
        snrThreshold: 119.93,
      });
      task.done();
    }
  );

  audit.define(
    { label: "Test 3", description: "Sine wave: 1 Hz" },
    async (task, should) => {
      let context = new OfflineAudioContext({
        length: durationFrames,
        sampleRate: sampleRate,
      });

      const freqHz = 1;

      let src = new OscillatorNode(context, {
        type: "sine",
        frequency: freqHz,
      });
      src.connect(context.destination);

      src.start();

      let renderedBuffer = await context.startRendering();
      checkResult(should, renderedBuffer, context, {
        freqHz: freqHz,
        a1: 0,
        b1: 1,
        prefix: "Sine",
        threshold: 1.4157e-7,
        snrThreshold: 112.22,
      });
      task.done();
    }
  );

  // audit.define(
  //   { label: "Test 4", description: "Custom wave: 100 Hz" },
  //   async (task, should) => {
  //     let context = new OfflineAudioContext({
  //       length: durationFrames,
  //       sampleRate: sampleRate,
  //     });

  //     const freqHz = 100;

  //     let wave = new PeriodicWave(context, {
  //       real: [0, 1],
  //       imag: [0, 1],
  //       disableNormalization: true,
  //     });
  //     let src = new OscillatorNode(context, {
  //       type: "custom",
  //       frequency: freqHz,
  //       periodicWave: wave,
  //     });
  //     src.connect(context.destination);

  //     src.start();

  //     let renderedBuffer = await context.startRendering();
  //     checkResult(should, renderedBuffer, context, {
  //       freqHz: freqHz,
  //       a1: 1,
  //       b1: 1,
  //       prefix: "Custom",
  //       threshold: 5.1e-5,
  //       snrThreshold: 112.6,
  //     });
  //     task.done();
  //   }
  // );

  // audit.define(
  //   { label: "Test 5", description: "Custom wave: 1 Hz" },
  //   async (task, should) => {
  //     let context = new OfflineAudioContext({
  //       length: durationFrames,
  //       sampleRate: sampleRate,
  //     });

  //     const freqHz = 1;

  //     let wave = new PeriodicWave(context, {
  //       real: [0, 1],
  //       imag: [0, 1],
  //       disableNormalization: true,
  //     });
  //     let src = new OscillatorNode(context, {
  //       type: "custom",
  //       frequency: freqHz,
  //       periodicWave: wave,
  //     });
  //     src.connect(context.destination);

  //     src.start();

  //     let renderedBuffer = await context.startRendering();
  //     checkResult(should, renderedBuffer, context, {
  //       freqHz: freqHz,
  //       a1: 1,
  //       b1: 1,
  //       prefix: "Custom",
  //       threshold: 4.7684e-7,
  //       snrThreshold: 133.0,
  //     });
  //     task.done();
  //   }
  // );

  audit.run();

  function waveForm(context, freqHz, a1, b1, nsamples) {
    let buffer = new AudioBuffer({
      length: nsamples,
      sampleRate: context.sampleRate,
    });
    let signal = buffer.getChannelData(0);
    const omega = (2 * Math.PI * freqHz) / context.sampleRate;
    for (let k = 0; k < nsamples; ++k) {
      signal[k] = a1 * Math.cos(omega * k) + b1 * Math.sin(omega * k);
    }

    return buffer;
  }

  function checkResult(should, renderedBuffer, context, options) {
    let { freqHz, a1, b1, prefix, threshold, snrThreshold } = options;

    let actual = renderedBuffer.getChannelData(0);

    let expected = waveForm(
      context,
      freqHz,
      a1,
      b1,
      actual.length
    ).getChannelData(0);

    should(actual, `${prefix}: ${freqHz} Hz`).beCloseToArray(expected, {
      absoluteThreshold: threshold,
    });

    let snr = 10 * Math.log10(computeSNR(actual, expected));

    should(snr, `${prefix}: SNR (db)`).beGreaterThanOrEqualTo(snrThreshold);
  }

  return waitUntilTestComplete(globalThis, testName);
}

module.exports = { wptOscillatorTest1 };
