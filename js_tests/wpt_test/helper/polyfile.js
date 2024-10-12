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

module.exports = { getDomException };
