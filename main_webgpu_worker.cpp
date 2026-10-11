#include "webgpu_feature_test.h"
#include "test_result.h"
#include <atomic>
#include <cstdio>
#include <cstdint>
#include <emscripten/wasm_worker.h>

namespace {
emscripten_wasm_worker_t worker{};
std::atomic<bool> passed{};
char detail[256]{};
bool result_sent{};

auto finished()->void {
  bool const success{passed.load()};
  emscripten_terminate_wasm_worker(worker);
  test_result(success, detail);
}

auto report(bool success, char const *message)->void {
  if(result_sent) return;
  result_sent = true;
  std::snprintf(detail, sizeof(detail), "%s", message);
  passed.store(success);
  emscripten_wasm_worker_post_function_v(EMSCRIPTEN_WASM_WORKER_ID_PARENT, finished);
}

auto run()->void {
  auto buffer{gpu_test::buffer(4, wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::CopySrc)};
  std::uint32_t const value{42};
  gpu_test::device.GetQueue().WriteBuffer(buffer, 0, &value, sizeof(value));
  gpu_test::verify(buffer, 4, [](void const *data) {
    return emscripten_current_thread_is_wasm_worker() && *static_cast<std::uint32_t const *>(data) == 42;
  }, "Worker device + GPU readback verified");
}

auto start_worker()->void {
  if(!EM_ASM_INT({ return !!navigator.gpu; })) {
    report(false, "WebGPU unavailable in worker");
    return;
  }
  gpu_test::start(std::nullopt, run, report);
}
}
auto main()->int {
  worker = emscripten_malloc_wasm_worker(1024 * 1024);
  if(!worker) return test_result(false, "Could not create Wasm Worker");
  emscripten_wasm_worker_post_function_v(worker, start_worker);
  return 0;
}
