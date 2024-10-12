const { doWaveValid } = require("./wave_producer/wave_valid");

const PYTHON_PATH = "/usr/local/Cellar/python@3.12/3.12.2_1/bin/python3";

function doValid(pythonPath) {
  console.log("Validating start...");
  const freq = 1;
  const sampleRate = 48000;
  const sign = 1;
  const sampleSize = sampleRate;
  if (!doWaveValid(pythonPath, freq, sampleRate, sign, sampleSize)) {
    return false;
  }
}

doValid(PYTHON_PATH);
