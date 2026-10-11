'use strict';
const features = [
  ['basic', 'Basic WebAssembly execution', 'C++26 main · arithmetic · pointer width', 'wasm'],
  ['storage', 'Browser storage', 'Temporary write / read / remove'],
  ['clipboard', 'Clipboard', 'Copy / paste · user action required'],
  ['fetch', 'Fetch & preloaded files', 'Separate download and packaged-file checks', 'fetch'],
  ['websocket', 'WebSockets', 'Capability only · no server connection', 'websocket'],
  ['dom', 'Embind & DOM', 'Basic execution → C++ / JavaScript round trip'],
  ['bulk_memory', 'Bulk memory', 'Basic execution → memory copy / fill', 'bulk_memory'],
  ['memory_growth', 'Memory growth', 'Basic execution → grow and verify heap', 'memory_growth'],
  ['maximum_memory', 'Custom maximum memory', 'Configured cap · not a capacity stress test'],
  ['stack', 'Custom main stack', 'Basic execution → 5 MiB stack', 'configuration'],
  ['exceptions_legacy', 'Wasm exceptions · legacy', 'Basic execution → throw / catch / unwind', 'exceptions_legacy'],
  ['exceptions', 'Wasm exceptions · standardized', 'Basic execution → try_table', 'exceptions'],
  ['simd', 'SIMD128', 'Basic execution → vector arithmetic', 'simd'],
  ['relaxed_simd', 'Relaxed SIMD / FMA', 'SIMD128 → relaxed instructions'],
  ['atomics', 'Shared memory & atomics', 'Isolation → shared memory → atomic operations', 'shared'],
  ['workers', 'Wasm Workers', 'Shared memory → worker dispatch · 1 MiB stack', 'worker'],
  ['synchronisation', 'Worker / worklet synchronization', 'Shared memory → locking and notification'],
  ['pthreads', 'Pthread creation', 'Optional diagnostic · separate from synchronization'],
  ['audio', 'Wasm audio worklets', 'Workers → audio processor · 4 KiB stack · click to start', 'audio'],
  ['webgpu', 'WebGPU / emdawnwebgpu', 'Basic execution → adapter → device → green canvas', 'gpu'],
  ['gpu_features', 'WebGPU feature extensions', 'Separate depth/stencil, BC, indirect and timestamp checks']
];
const implemented = new Set(window.feature_tests || []);
const runs = new Map();
const tbody = document.getElementById('tests');
// Minimal wasm32 modules isolate instruction support from Emscripten startup.
// All section lengths below fit in a single LEB128 byte.
function validate_instructions(instructions, memory = false) {
  const body = [0, ...instructions, 0x0b]; // No locals; end function.
  return typeof WebAssembly === 'object' && WebAssembly.validate(new Uint8Array([
    0, 97, 115, 109, 1, 0, 0, 0,
    1, 4, 1, 0x60, 0, 0, // One function type: () -> ().
    3, 2, 1, 0,
    ...(memory ? [5, 3, 1, 0, 1] : []), // One page of unshared wasm32 memory.
    10, body.length + 2, 1, body.length, ...body
  ]));
}
function instruction_detection(instructions, memory = false) {
  return validate_instructions(instructions, memory) ? 'Instruction probe accepted' : 'Instruction probe rejected';
}
function detection(probe) {
  try {
    switch(probe) {
      case 'wasm': return typeof WebAssembly === 'object' && WebAssembly.validate(new Uint8Array([0,97,115,109,1,0,0,0])) ? 'Wasm available · architecture verified alongside' : 'WebAssembly unavailable';
      case 'exceptions_legacy':
        // try (void), nop, catch_all, nop, end.
        return instruction_detection([0x06, 0x40, 0x01, 0x19, 0x01, 0x0b]);
      case 'exceptions':
        // try_table (void), zero catch clauses, nop, end.
        return instruction_detection([0x1f, 0x40, 0, 0x01, 0x0b]);
      case 'simd':
        // v128.const zero, drop.
        return instruction_detection([0xfd, 0x0c, ...Array(16).fill(0), 0x1a]);
      case 'bulk_memory':
        // Three i32 arguments per operation; memory.copy then memory.fill.
        return instruction_detection([
          0x41, 0, 0x41, 0, 0x41, 0, 0xfc, 10, 0, 0,
          0x41, 0, 0x41, 0, 0x41, 0, 0xfc, 11, 0
        ], true);
      case 'memory_growth': {
        const memory = new WebAssembly.Memory({initial: 1, maximum: 2});
        new Uint8Array(memory.buffer)[0] = 42;
        const previous = memory.grow(1);
        return previous === 1 && memory.buffer.byteLength === 131072 && new Uint8Array(memory.buffer)[0] === 42
          ? 'JS memory growth verified' : 'JS memory growth failed';
      }
      case 'configuration': return 'Build setting · verified by program';
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
  document.getElementById('summary').textContent = `${passed} passed · ${failed} failed · ${running} running · ${features.filter(([id]) => !implemented.has(id)).length} feature rows planned`;
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
      if(!implemented.has(id)) {
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
