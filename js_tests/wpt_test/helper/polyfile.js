// refer to https://github.com/nodejs/undici/issues/1740#issuecomment-1296467885
function getDomException() {
  return (
    globalThis.DOMException ??
    (() => {
      // DOMException was only made a global in Node v17.0.0,
      // but fetch supports >= v16.8.
      try {
        atob("~");
      } catch (err) {
        return Object.getPrototypeOf(err).constructor;
      }
    })()
  );
}

async function waitUntilTestComplete(self, testName, cb) {
  return new Promise((resolve, reject) => {
    self.add_completion_callback((testRet) => {
      doTestCompleteCheckPassCb(testRet, testName);
      if (cb) {
        cb(testRet);
      }
      resolve(testRet);
    });
  });
}

function doTestCompleteCheckPassCb(testRet, testName) {
  const { retFormatter } = require("js_tests/wpt_test/helper/ret_formatter.js");
  const formatRet = retFormatter(testRet);
  let unpassRecords = [];
  for (let i = 0; i < formatRet.length; i++) {
    if (formatRet[i]["status"] != "Pass") {
      unpassRecords.push(formatRet[i]);
    }
  }
  if (unpassRecords.length) {
    console.log(`❌ ${testName} failed!`);
    console.log(unpassRecords);
  } else {
    console.log(`✅ ${testName} passed!`);
  }
}

function requireReload(module) {
  delete require.cache[require.resolve(module)];
  return require(module);
}

module.exports = {
  getDomException,
  doTestCompleteCheckPassCb,
  waitUntilTestComplete,
  requireReload,
};
