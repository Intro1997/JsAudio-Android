/**
 * detail description is defined in testharness.js
 * following is a sample:
 * {
 *    name: '# AUDIT TASK RUNNER STARTED.',
 *    phase: 4, // defined in Test.prototype.phases of testharness.js
 *    status: 0, // defined in Test.prototype.status_formats of testharness.js
 *    timeout_id: null,
 *    index: 1, // test id, start from 1
 *    properties: {},
 *    timeout_length: null,
 *    message: null,
 *    stack: null,
 *    steps: [ [Function (anonymous)] ],
 *    _is_promise_test: false,
 *    cleanup_callbacks: [],
 *    _user_defined_cleanup_count: 0,
 *    _done_callbacks: [],
 *    _abortController: AbortController { signal: AbortSignal { aborted: true } },
 *    current_test: null
 * },
 */

const phaseConverter = [
  "INITIAL", // 0
  "SETUP", // 1
  "HAVE_TESTS", // 2
  "HAVE_RESULTS", // 3
  "COMPLETE", // 4
];
const StatusConverter = [
  "Pass", // 0
  "Fail", // 1
  "Timeout", // 2
  "Not Run", // 3
  "Optional Feature Unsupported", // 4
]; // 4

function retFormatter(input_arr) {
  for (let i = 0; i < input_arr.length; i++) {
    if (input_arr[i]["phase"] != null) {
      input_arr[i]["phase"] = phaseConverter[input_arr[i]["phase"]];
    }
    if (input_arr[i]["status"] != null) {
      input_arr[i]["status"] = StatusConverter[input_arr[i]["status"]];
    }
  }
  return input_arr;
}
module.exports = { retFormatter };
