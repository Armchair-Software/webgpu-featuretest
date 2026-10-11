'use strict';
const parameters = new URLSearchParams(location.search);
const target = parameters.get('target');
let finished = false;
window.report_result = (passed, detail) => {
  if(finished) return;
  finished = true;
  const state = passed ? 'passed' : 'failed';
  document.body.dataset.state = state;
  document.getElementById('status').textContent = passed ? '✓ Execution verified' : '× Execution failed';
  parent.postMessage({type: 'feature-result', target, state, detail: String(detail)}, location.origin);
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
