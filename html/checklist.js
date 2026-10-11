'use strict';
const features = [
  ['basic', 'Basic WebAssembly execution', 'C++26 main · arithmetic · pointer width', 'wasm'],
  ['exceptions_legacy', 'Wasm exceptions · legacy', 'Basic execution → throw / catch / unwind'],
  ['exceptions', 'Wasm exceptions · standardized', 'Basic execution → try_table'],
  ['simd', 'SIMD128', 'Basic execution → vector arithmetic'],
  ['relaxed_simd', 'Relaxed SIMD / FMA', 'SIMD128 → relaxed instructions'],
  ['bulk_memory', 'Bulk memory', 'Basic execution → memory copy / fill'],
  ['memory_growth', 'Memory growth', 'Basic execution → grow and verify heap'],
  ['maximum_memory', 'Custom maximum memory', 'Configured cap · not a capacity stress test'],
  ['stack', 'Custom main stack', 'Basic execution → 5 MiB stack'],
  ['atomics', 'Shared memory & atomics', 'Isolation → shared memory → atomic operations', 'shared'],
  ['workers', 'Wasm Workers', 'Shared memory → worker dispatch · 1 MiB stack', 'worker'],
  ['synchronisation', 'Worker / worklet synchronization', 'Shared memory → locks · emscripten_sync.h'],
  ['pthreads', 'Pthread creation', 'Optional diagnostic · separate from synchronization'],
  ['webgpu', 'WebGPU / emdawnwebgpu', 'Basic execution → adapter → device → green canvas', 'gpu'],
  ['gpu_features', 'WebGPU feature extensions', 'Separate depth/stencil, BC, indirect and timestamp checks'],
  ['audio', 'Wasm audio worklets', 'Workers → audio processor · 4 KiB stack · click to start', 'audio'],
  ['fetch', 'Fetch & preloaded files', 'Separate download and packaged-file checks', 'fetch'],
  ['websocket', 'WebSockets', 'Raindrop · capability only; no server connection', 'websocket'],
  ['dom', 'Embind & DOM', 'Basic execution → C++ / JavaScript round trip'],
  ['storage', 'Browser storage', 'Skilltree → temporary write / read / remove'],
  ['clipboard', 'Clipboard', 'Shared emscripten_browser_clipboard.h · user action'],
  ['ports', 'GLFW / PNG / bzip2', 'Raindrop → separate library integration checks']
];
const runs = new Map();
const tbody = document.getElementById('tests');
function detection(probe) {
  try {
    switch(probe) {
      case 'wasm': return typeof WebAssembly === 'object' && WebAssembly.validate(new Uint8Array([0,97,115,109,1,0,0,0])) ? 'Wasm available · architecture verified alongside' : 'WebAssembly unavailable';
      case 'shared': return crossOriginIsolated && typeof SharedArrayBuffer === 'function' && typeof Atomics === 'object' ? 'Shared-memory APIs exposed' : 'Isolation / shared-memory APIs unavailable';
      case 'worker': return typeof Worker === 'function' ? 'Worker API exposed' : 'Worker API unavailable';
      case 'gpu': return navigator.gpu ? 'WebGPU API exposed · adapter not tested' : 'WebGPU API unavailable';
      case 'audio': return typeof AudioWorkletNode === 'function' ? 'AudioWorklet API exposed' : 'AudioWorklet API unavailable';
      case 'fetch': return typeof fetch === 'function' ? 'Fetch API exposed' : 'Fetch API unavailable';
      case 'websocket': return typeof WebSocket === 'function' ? 'WebSocket API exposed · connectivity untested' : 'WebSocket API unavailable';
      default: return 'Probe planned';
    }
  } catch(error) { return `Check failed: ${error.message}`; }
}
function update_summary() {
  const values = [...runs.values()];
  const passed = values.filter(run => run.state === 'passed').length;
  const failed = values.filter(run => run.state === 'failed').length;
  const running = values.filter(run => run.state === 'running').length;
  document.getElementById('summary').textContent = `${passed} passed · ${failed} failed · ${running} running · ${features.length - 1} feature rows planned`;
  document.body.dataset.complete = String(running === 0);
}
function finish(run, state, detail) {
  if(run.state !== 'running') return;
  clearTimeout(run.timer);
  run.state = state;
  run.result.textContent = detail;
  run.result.classList.toggle('failed', state === 'failed');
  run.cell.dataset.state = state;
  if(state === 'failed') {
    // Replace the frame on failure/timeout, stopping its program and late messages.
    const preview = document.createElement('div');
    preview.className = 'preview';
    preview.style.background = '#a92d38';
    preview.style.color = 'white';
    preview.textContent = '× Execution failed';
    run.frame.replaceWith(preview);
  }
  update_summary();
}
window.addEventListener('message', event => {
  if(event.origin !== location.origin || event.data?.type !== 'feature-result') return;
  const run = runs.get(event.data.target);
  if(!run || event.source !== run.frame.contentWindow) return;
  if(!['passed', 'failed'].includes(event.data.state)) return;
  finish(run, event.data.state, String(event.data.detail));
});
function start() {
  for(const run of runs.values()) clearTimeout(run.timer);
  runs.clear();
  tbody.replaceChildren();
  for(const [id, title, description, probe] of features) {
    const row = document.createElement('tr');
    const name = row.insertCell();
    const heading = document.createElement('div');
    heading.className = 'feature';
    heading.textContent = title;
    const explanation = document.createElement('div');
    explanation.className = 'description';
    explanation.textContent = description;
    name.append(heading, explanation);
    const check = row.insertCell();
    check.className = 'detection';
    check.textContent = detection(probe);
    for(const bits of [32, 64]) {
      const cell = row.insertCell();
      if(id !== 'basic') {
        const placeholder = document.createElement('div');
        placeholder.className = 'preview';
        placeholder.textContent = 'Planned';
        cell.append(placeholder);
        continue;
      }
      const target = `${id}_wasm${bits}`;
      const frame = document.createElement('iframe');
      frame.title = `${title}, wasm${bits}`;
      frame.src = `runner.html?target=${target}`;
      const result = document.createElement('div');
      result.className = 'result';
      result.textContent = 'Loading independent program…';
      const run = {frame, result, cell, state: 'running'};
      runs.set(target, run);
      run.timer = setTimeout(() => finish(run, 'failed', 'Timed out after 15 seconds'), 15000);
      cell.append(frame, result);
    }
    tbody.append(row);
  }
  update_summary();
}
document.getElementById('context').textContent = isSecureContext ? 'Secure context' : 'Not a secure context';
document.getElementById('isolation').textContent = crossOriginIsolated ? 'Enabled · shared memory permitted' : 'Disabled · workers may be blocked';
const build = window.build_info;
document.getElementById('build').textContent = build ? `${build.standard} · ${build.configuration} · ${build.compiler}` : 'Run ./build.sh and serve the build directory';
document.getElementById('browser').textContent = navigator.userAgent;
document.getElementById('rerun').addEventListener('click', start);
start();
