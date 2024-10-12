const { execSync } = require("child_process");
const fs = require("fs");
const path = require("path");

const thisModulePath = __dirname;

function execCmdSync(cmdStr, execPath) {
  const originPath = process.cwd();
  process.chdir(execPath);

  let output = null;
  let error = null;

  try {
    const execOutput = execSync(cmdStr, {
      encoding: "utf-8",
    });
    output = execOutput;
  } catch (err) {
    error = {
      status: err.status,
      stdout: err.stdout,
    };
  }

  process.chdir(originPath);
  return {
    output,
    error,
  };
}

function doWaveValid(pythonPath, frequency, sampleRate, sign, sampleSize) {
  console.log("--- Wave Validation Process ---");
  const createValidationSetCmd = `${pythonPath} ${thisModulePath}/create_wave_validation_set.py ${frequency} ${sampleRate} ${sign} ${sampleSize}`;
  const compileValidateCmd = `bash ${thisModulePath}/compile_wave_valid.sh`;
  const startValidateCmd = `./build/wave_producer_valid`;

  let ret = execCmdSync(createValidationSetCmd, thisModulePath);
  if (ret.error) {
    console.error(`❌ create wave validation set error: ${ret.error}`);
    return false;
  }

  ret = execCmdSync(compileValidateCmd, thisModulePath);
  if (ret.error) {
    console.error(`❌ compile cpp validate application error: ${ret.error}`);
    return false;
  }

  ret = execCmdSync(
    `${startValidateCmd} ${frequency} ${sampleRate} ${sign} ${sampleRate}`,
    thisModulePath
  );
  if (ret.error) {
    console.error(`❌ do validate error: ${ret.error.stdout}`);
    return false;
  } else {
    console.log(`${ret.output}`);
  }
  console.log("✅ Do wave producer valid success!");
  console.log("--- Wave Validation End ---");

  return true;
}

module.exports = { doWaveValid };
