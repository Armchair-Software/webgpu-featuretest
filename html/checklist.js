'use strict';
const main_features = [
  ['canvas', 'Canvas 2D', 'Create context → fill → pixel readback', 'canvas'],
  ['basic', 'Basic WebAssembly execution', 'C++26 · pointer width · heap · i64/BigInt', 'wasm'],
  ['storage', 'Browser storage', 'Temporary localStorage write / read / remove', 'storage'],
  ['fetch', 'Fetch', 'C++ fetch → local fixture → verify bytes', 'fetch'],
  ['filesystem', 'Preloaded files', 'Packaged fixture → C++ virtual filesystem', 'configuration'],
  ['websocket', 'WebSockets', 'Capability only · no server connection', 'websocket'],
  ['dom', 'Embind & DOM', 'Basic execution → C++ / JavaScript round trip', 'dom'],
  ['bulk_memory', 'Bulk memory', 'Basic execution → memory copy / fill', 'bulk_memory'],
  ['memory_growth', 'Memory growth', 'Basic execution → grow and verify heap', 'memory_growth'],
  ['maximum_memory_2gb', 'Maximum memory · 2 GiB', 'MAXIMUM_MEMORY=2GB · 16 MiB initial heap', 'maximum_memory_2gb'],
  ['maximum_memory_4gb', 'Maximum memory · 4 GiB', 'MAXIMUM_MEMORY=4GB · 16 MiB initial heap', 'maximum_memory_4gb'],
  ['maximum_memory_8gb', 'Maximum memory · 8 GiB', 'MAXIMUM_MEMORY=8GB · 16 MiB initial heap', 'maximum_memory_8gb'],
  ['stack', 'Custom main stack', 'Basic execution → 5 MiB stack', 'configuration'],
  ['exceptions_js', 'C++ exceptions · JavaScript', 'JS-based throw / catch / rethrow / unwind', 'exceptions_js'],
  ['exceptions_legacy', 'Wasm exceptions · legacy', 'Basic execution → throw / catch / unwind', 'exceptions_legacy'],
  ['exceptions', 'Wasm exceptions · standardized', 'Basic execution → try_table', 'exceptions'],
  ['simd', 'SIMD128', 'Basic execution → vector arithmetic', 'simd'],
  ['relaxed_simd', 'Relaxed SIMD / FMA', 'SIMD128 → relaxed instructions', 'relaxed_simd'],
  ['atomics', 'Shared memory & atomics', 'Isolation → shared memory → atomic operations', 'shared'],
  ['workers', 'Wasm Workers', 'Shared memory → worker dispatch · 1 MiB stack', 'worker'],
  ['synchronisation', 'Worker / worklet synchronization', 'Wasm Workers + pthread support → shared locks', 'shared'],
  ['pthreads', 'Pthread creation', 'Optional diagnostic · separate from synchronization', 'shared'],
  ['audio_buffer', 'Web Audio · offline rendering', 'Audio buffer → graph → verify rendered samples · silent', 'audio_buffer'],
  ['audio', 'Wasm audio worklets', 'Workers → audio processor · report autoplay restrictions', 'audio'],
  ['webgl', 'WebGL 1', 'Create context → green clear → pixel readback', 'webgl'],
  ['webgl2', 'WebGL 2', 'Create context → green clear → pixel readback', 'webgl2'],
  ['webgpu', 'WebGPU / emdawnwebgpu', 'Basic execution → adapter → device → green canvas', 'gpu'],
  ['webgpu_compute', 'WebGPU · basic compute', 'f32 / u32 shader arithmetic → buffer readback', 'gpu'],
  ['webgpu_render', 'WebGPU · shader rendering', 'Vertex / fragment shaders → green pixel readback', 'gpu'],
  ['webgpu_indexed', 'WebGPU · indexed drawing', 'Vertex / index buffers → draw → pixel readback', 'gpu'],
  ['webgpu_uniform', 'WebGPU · uniform buffers', 'Update uniform → vertex / fragment shader reads', 'gpu'],
  ['webgpu_bundles', 'WebGPU · render bundles', 'Record draw commands → replay → pixel readback', 'gpu'],
  ['webgpu_worker', 'WebGPU · Wasm worker', 'Worker → adapter → device → GPU readback', 'gpu_worker'],
  ['webgpu_depth_stencil', 'WebGPU · depth32float-stencil8', 'Depth/stencil texture and render pass', 'gpu:depth32float-stencil8'],
  ['webgpu_bc', 'WebGPU · texture-compression-bc', 'Upload and sample a BC-compressed texture', 'gpu:texture-compression-bc'],
  ['webgpu_indirect', 'WebGPU · indirect-first-instance', 'Indirect draw with non-zero first instance', 'gpu:indirect-first-instance'],
  ['webgpu_timestamp', 'WebGPU · timestamp-query', 'Write and resolve GPU timestamps', 'gpu:timestamp-query'],
];
const optional_features = [
  ['webgpu_etc2', 'WebGPU · texture-compression-etc2', 'Upload and sample an ETC2-compressed texture', 'gpu:texture-compression-etc2'],
  ['webgpu_astc', 'WebGPU · texture-compression-astc', 'Upload and sample an ASTC-compressed texture', 'gpu:texture-compression-astc'],
  ['webgpu_f16', 'WebGPU · shader-f16', 'Execute a shader using half-precision floats', 'gpu:shader-f16'],
  ['webgpu_float32_filterable', 'WebGPU · float32-filterable', 'Filter a 32-bit float texture', 'gpu:float32-filterable'],
  ['webgpu_bgra8unorm_storage', 'WebGPU · bgra8unorm-storage', 'Compute shader → BGRA texture → readback', 'gpu:bgra8unorm-storage'],
  ['webgpu_rg11b10ufloat', 'WebGPU · rg11b10ufloat-renderable', 'Render into a packed float texture', 'gpu:rg11b10ufloat-renderable'],
  ['webgpu_float32_blendable', 'WebGPU · float32-blendable', 'Blend colours into a 32-bit float texture', 'gpu:float32-blendable'],
  ['webgpu_depth_clip_control', 'WebGPU · depth-clip-control', 'Compare clipped and unclipped geometry', 'gpu:depth-clip-control'],
  ['webgpu_subgroups', 'WebGPU · subgroups', 'Collective shader arithmetic → readback', 'gpu:subgroups'],
  ['webgpu_clip_distances', 'WebGPU · clip-distances', 'Shader clip plane → pixel readback', 'gpu:clip-distances'],
  ['webgpu_dual_source_blending', 'WebGPU · dual-source-blending', 'Blend using two fragment shader outputs', 'gpu:dual-source-blending']
];
const features = [...main_features, ...optional_features];
const modes = window.test_modes;
const implemented = new Set(window.feature_tests || []);
const unavailable = {
  maximum_memory_8gb_wasm32: 'Wasm32 address space is limited to 4 GiB',
  maximum_memory_8gb_wasm64_lowered: 'Lowered addressing is limited to 4 GiB'
};
const runs = new Map();
let next_timer;
const tbody = document.getElementById('tests');
const optional_tbody = document.getElementById('optional-tests');
let gpu_adapter;
async function detect_gpu_feature(name) {
  if(!navigator.gpu) return 'WebGPU API unavailable';
  gpu_adapter ??= navigator.gpu.requestAdapter();
  const adapter = await gpu_adapter;
  if(!adapter) return 'WebGPU adapter unavailable';
  return adapter.features.has(name) ? 'Adapter exposes extension' : 'Extension unavailable';
}
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
function memory_limit_detection(gib) {
  const pages = gib * 1024 * 1024 * 1024 / 65536;
  return [32, 64].map(bits => {
    if(bits === 32 && gib > 4) return 'memory32: not applicable';
    try {
      const descriptor = bits === 64
        ? {address: 'i64', initial: BigInt(1), maximum: BigInt(pages)}
        : {address: 'i32', initial: 1, maximum: pages};
      const memory = new WebAssembly.Memory(descriptor);
      // Only one page is allocated. Acceptance does not promise growth to the cap.
      return `memory${bits}: ${memory.buffer.byteLength === 65536 ? 'accepted' : 'unexpected size'}`;
    } catch(error) {
      return `memory${bits}: rejected (${error.name})`;
    }
  }).join(' · ');
}
function detection(probe) {
  try {
    switch(probe) {
      case 'wasm': {
        if(typeof WebAssembly !== 'object' || typeof BigInt !== 'function' || typeof BigInt64Array !== 'function') {
          return 'Wasm / BigInt APIs unavailable';
        }
        // Wasm32 memory and an i64 return value: no native memory64 requirement.
        const module = new WebAssembly.Module(new Uint8Array([
          0, 97, 115, 109, 1, 0, 0, 0,
          1, 5, 1, 0x60, 0, 1, 0x7e, // () -> i64
          3, 2, 1, 0,                 // Function using type 0.
          5, 3, 1, 0, 1,             // One page of wasm32 memory.
          7, 5, 1, 1, 0x66, 0, 0,    // Export function 0 as "f".
          10, 6, 1, 4, 0, 0x42, 42, 0x0b // i64.const 42
        ]));
        return new WebAssembly.Instance(module).exports.f() === BigInt(42)
          ? 'Wasm + i64/BigInt interop verified' : 'Wasm i64/BigInt interop failed';
      }
      case 'exceptions_legacy':
        // try (void), nop, catch_all, nop, end.
        return instruction_detection([0x06, 0x40, 0x01, 0x19, 0x01, 0x0b]);
      case 'exceptions':
        // try_table (void), zero catch clauses, nop, end.
        return instruction_detection([0x1f, 0x40, 0, 0x01, 0x0b]);
      case 'simd':
        // v128.const zero, drop.
        return instruction_detection([0xfd, 0x0c, ...Array(16).fill(0), 0x1a]);
      case 'relaxed_simd': {
        // Three v128.const operands, f32x4.relaxed_madd (0x105), drop.
        const zero = [0xfd, 0x0c, ...Array(16).fill(0)];
        return instruction_detection([...zero, ...zero, ...zero, 0xfd, 0x85, 0x02, 0x1a]);
      }
      case 'canvas': return typeof CanvasRenderingContext2D === 'function' ? 'Canvas 2D API exposed' : 'Canvas 2D API unavailable';
      case 'webgl': return typeof WebGLRenderingContext === 'function' ? 'WebGL 1 API exposed · context tested alongside' : 'WebGL 1 API unavailable';
      case 'webgl2': return typeof WebGL2RenderingContext === 'function' ? 'WebGL 2 API exposed · context tested alongside' : 'WebGL 2 API unavailable';
      case 'storage': return typeof window.localStorage?.getItem === 'function' ? 'Storage API exposed' : 'Storage API unavailable';
      case 'dom': return typeof document.createElement === 'function' ? 'DOM API exposed' : 'DOM API unavailable';
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
      case 'maximum_memory_2gb': return memory_limit_detection(2);
      case 'maximum_memory_4gb': return memory_limit_detection(4);
      case 'maximum_memory_8gb': return memory_limit_detection(8);
      case 'configuration': return 'Build setting · verified by program';
      case 'shared': return crossOriginIsolated && typeof SharedArrayBuffer === 'function' && typeof Atomics === 'object' ? 'Shared-memory APIs exposed' : 'Isolation / shared-memory APIs unavailable';
      case 'worker': return typeof Worker === 'function' ? 'Worker API exposed' : 'Worker API unavailable';
      case 'gpu_worker': return navigator.gpu && typeof Worker === 'function' && crossOriginIsolated ? 'GPU + isolated worker APIs exposed' : 'GPU / isolated worker APIs unavailable';
      case 'exceptions_js': return 'Emscripten JS runtime · no native EH required';
      case 'gpu': return navigator.gpu ? 'WebGPU API exposed · adapter not tested' : 'WebGPU API unavailable';
      case 'audio_buffer': return `AudioContext ${typeof AudioContext === 'function' ? 'exposed' : 'unavailable'} · OfflineAudioContext ${typeof OfflineAudioContext === 'function' ? 'exposed' : 'unavailable'}`;
      case 'audio': return typeof AudioWorkletNode === 'function' ? 'AudioWorklet API exposed' : 'AudioWorklet API unavailable';
      case 'fetch': return typeof XMLHttpRequest === 'function' ? 'XHR API exposed · used by Emscripten fetch' : 'XHR API unavailable';
      case 'websocket': return typeof WebSocket === 'function' ? 'WebSocket API exposed · connectivity untested' : 'WebSocket API unavailable';
      default: return 'Probe planned';
    }
  } catch(error) { return `Check failed: ${error.message}`; }
}
function update_summary() {
  const values = [...runs.values()];
  const passed = values.filter(run => run.state === 'passed').length;
  const failed = values.filter(run => run.state === 'failed').length;
  const blocked = values.filter(run => run.state === 'blocked').length;
  const running = values.filter(run => run.state === 'running').length;
  const queued = values.filter(run => run.state === 'queued').length;
  document.getElementById('summary').textContent = `${passed} passed · ${failed} failed · ${blocked} blocked · ${running} running${queued ? ` · ${queued} queued` : ''}`;
  document.body.dataset.complete = String(running === 0 && queued === 0);
  document.getElementById('download').disabled = running !== 0 || queued !== 0;
}
function start_next() {
  if([...runs.values()].some(run => run.state === 'running')) return;
  const run = [...runs.values()].find(run => run.state === 'queued');
  if(!run) return;
  run.state = 'running';
  run.cell.dataset.state = 'running';
  run.result.textContent = 'Loading independent program…';
  run.frame = document.createElement('iframe');
  run.frame.title = run.title;
  run.frame.src = `runner.html?target=${run.target}`;
  run.preview.replaceWith(run.frame);
  run.timer = setTimeout(() => finish(run, 'failed', 'Timed out after 15 seconds'), 15000);
  update_summary();
}
function cleanup_run(run) {
  clearTimeout(run.timer);
  try {
    run.frame?.contentWindow.cleanup_test?.();
  } catch(error) {
    console.error('Test cleanup failed:', run.target, error);
  }
}
function stop() {
  clearTimeout(next_timer);
  for(const run of runs.values()) {
    cleanup_run(run);
    run.frame?.remove();
    run.frame = null;
  }
}
window.addEventListener('pagehide', stop);
window.addEventListener('pageshow', event => { if(event.persisted) start(); });
function finish(run, state, detail) {
  if(run.state !== 'running') return;
  clearTimeout(run.timer);
  run.state = state;
  run.result.textContent = detail;
  run.result.classList.toggle('failed', state === 'failed');
  run.cell.dataset.state = state;
  if(state === 'blocked') {
    // Keep the suspended context so the click can resume it synchronously.
    run.frame.style.display = 'none';
    const button = document.createElement('button');
    button.className = 'audio-retry';
    button.textContent = 'Click to test';
    button.addEventListener('click', retry_audio);
    run.cell.prepend(button);
  } else {
    cleanup_run(run);
    // Retain the visual result, but release the Wasm heap, workers and GPU context.
    const preview = document.createElement('div');
    preview.className = 'preview completed';
    preview.style.background = state === 'passed' ? '#168249' : '#a92d38';
    preview.style.color = 'white';
    preview.textContent = state === 'passed' ? '✓ Execution verified' : '× Execution failed';
    run.frame.replaceWith(preview);
    run.preview = preview;
    run.frame = null;
  }
  update_summary();
  clearTimeout(next_timer);
  next_timer = setTimeout(start_next, 50);
}
function retry_failed(run) {
  if(run.state !== 'failed') return;
  const preview = document.createElement('div');
  preview.className = 'preview';
  preview.textContent = 'Queued';
  run.preview.replaceWith(preview);
  run.preview = preview;
  run.state = 'queued';
  run.cell.dataset.state = 'queued';
  run.result.classList.remove('failed');
  run.result.textContent = 'Waiting for previous test…';
  update_summary();
  clearTimeout(next_timer);
  next_timer = setTimeout(start_next, 50);
}
function retry_audio() {
  for(const run of runs.values()) {
    if(run.state !== 'blocked') continue;
    run.cell.querySelector('.audio-retry').remove();
    run.frame.style.display = '';
    run.state = 'running';
    run.cell.dataset.state = 'running';
    run.result.textContent = 'Testing silent audio…';
    run.timer = setTimeout(() => finish(run, 'failed', 'Timed out after 15 seconds'), 15000);
    try {
      // A direct same-origin call preserves the click's user activation.
      run.frame.contentWindow.resume_audio_test();
    } catch(error) {
      finish(run, 'failed', String(error));
    }
  }
  update_summary();
}
window.addEventListener('message', event => {
  if(event.origin !== location.origin || event.data?.type !== 'feature-result') return;
  const run = runs.get(event.data.target);
  if(!run || event.source !== run.frame?.contentWindow) return;
  if(!['passed', 'failed', 'blocked'].includes(event.data.state)) return;
  finish(run, event.data.state, String(event.data.detail));
});
function start() {
  stop();
  runs.clear();
  tbody.replaceChildren();
  optional_tbody.replaceChildren();
  gpu_adapter = undefined;
  for(const [id, title, description, probe] of features) {
    const row = document.createElement('tr');
    row.dataset.feature = id;
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
    if(probe?.startsWith('gpu:')) {
      check.textContent = 'Checking adapter…';
      detect_gpu_feature(probe.slice(4)).then(text => { check.textContent = text; })
        .catch(error => { check.textContent = `Check failed: ${error.message}`; });
    } else {
      check.textContent = detection(probe);
    }
    for(const mode of modes) {
      const cell = row.insertCell();
      cell.dataset.mode = mode.label;
      const target = `${id}_${mode.id}`;
      if(unavailable[target]) {
        cell.dataset.state = 'unavailable';
        const label = document.createElement('div');
        label.className = 'preview';
        label.textContent = 'Not applicable';
        const detail = document.createElement('div');
        detail.className = 'result';
        detail.textContent = unavailable[target];
        cell.append(label, detail);
        continue;
      }
      if(!implemented.has(id)) {
        const placeholder = document.createElement('div');
        placeholder.className = 'preview';
        placeholder.textContent = 'Planned';
        cell.append(placeholder);
        continue;
      }
      const preview = document.createElement('div');
      preview.className = 'preview';
      preview.textContent = 'Queued';
      const result = document.createElement('div');
      result.className = 'result';
      result.textContent = 'Waiting for previous test…';
      cell.dataset.state = 'queued';
      const run = {target, title: `${title}, ${mode.label}`, preview, frame: null, result, cell, state: 'queued'};
      cell.addEventListener('click', () => retry_failed(run));
      runs.set(target, run);
      cell.append(preview, result);
    }
    const section = optional_features.some(([feature]) => feature === id) ? optional_tbody : tbody;
    section.append(row);
  }
  update_summary();
  next_timer = setTimeout(start_next, 50);
}
function create_report() {
  return {
    generated_at: new Date().toISOString(),
    user_agent: navigator.userAgent,
    secure_context: isSecureContext,
    cross_origin_isolated: crossOriginIsolated,
    tests: Object.fromEntries(features.flatMap(([id, name]) => {
      const check = document.querySelector(`tr[data-feature="${id}"] .detection`).textContent;
      return modes.map(mode => {
        const target = `${id}_${mode.id}`;
        const run = runs.get(target);
        const state = run?.state || (unavailable[target] ? 'unavailable' : 'planned');
        const detail = run?.result.textContent || unavailable[target] || 'Not implemented';
        return [`${name} (${mode.label})`, `${state}: ${detail}, JS: ${check}`];
      });
    }))
  };
}
function download_report() {
  const report = create_report();
  const blob = new Blob([JSON.stringify(report, null, 2) + '\n'], {type: 'application/json'});
  const url = URL.createObjectURL(blob);
  const link = document.createElement('a');
  link.href = url;
  link.download = `browser-feature-report-${report.generated_at.replace(/[:.]/g, '-')}.json`;
  document.body.append(link);
  link.click();
  link.remove();
  setTimeout(() => URL.revokeObjectURL(url), 1000);
}
document.getElementById('context').textContent = isSecureContext ? 'Secure context' : 'Not a secure context';
document.getElementById('isolation').textContent = crossOriginIsolated ? 'Enabled · shared memory permitted' : 'Disabled · workers may be blocked';
const build = window.build_info;
document.getElementById('build').textContent = build ? `Emscripten ${build.emscripten} · ${build.compiler} · ${build.standard} · ${build.configuration}` : 'Run ./build.sh and serve the build directory';
document.getElementById('browser').textContent = navigator.userAgent;
document.getElementById('download').addEventListener('click', download_report);
document.getElementById('rerun').addEventListener('click', start);
start();
