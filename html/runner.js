'use strict';
const parameters = new URLSearchParams(location.search);
const target = parameters.get('target');
let finished = false;
function report(state, detail) {
  if(finished) return;
  finished = true;
  document.body.dataset.state = state;
  document.getElementById('status').textContent = state === 'passed' ? '✓ Execution verified' : state === 'blocked' ? '— Audio suspended' : '× Execution failed';
  parent.postMessage({type: 'feature-result', target, state, detail: String(detail)}, location.origin);
}
window.report_result = (passed, detail) => report(passed ? 'passed' : 'failed', detail);
window.report_blocked = detail => report('blocked', detail);
window.resume_audio_test = () => {
  if(document.body.dataset.state !== 'blocked') return;
  finished = false;
  document.body.dataset.state = 'running';
  document.getElementById('status').textContent = 'Running…';
  Module._resume_audio_test();
};
window.addEventListener('error', event => window.report_result(false, event.message));
window.addEventListener('unhandledrejection', event => window.report_result(false, String(event.reason)));
window.Module = {
  onAbort: reason => window.report_result(false, String(reason)),
  print: text => console.log(text),
  printErr: text => console.error(text)
};
const match = /^(\w+)_wasm(32|64)$/.exec(target || '');
if(!match || !window.feature_tests?.includes(match[1])) {
  window.report_result(false, 'Unknown test target');
} else {
  const script = document.createElement('script');
  script.src = `${target}.js`;
  script.onerror = () => window.report_result(false, `Cannot load ${target}.js; run ./build.sh`);
  document.body.append(script);
}
