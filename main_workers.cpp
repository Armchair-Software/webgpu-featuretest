#include <atomic>
#include <cstddef>
#include <emscripten/stack.h>
#include <emscripten/wasm_worker.h>
#include "test_result.h"

namespace {
emscripten_wasm_worker_t worker{};
std::atomic<int> result{};

// Called on the parent after the worker has published its result.
auto finished()->void {
  bool const passed{result.load() == 42};
  emscripten_terminate_wasm_worker(worker);
  test_result(passed, passed ? "Worker round trip · 1 MiB stack verified" : "Worker execution or stack check failed");
}

auto run_worker()->void {
  unsigned char volatile bytes[64 * 1024];
  bool passed{emscripten_current_thread_is_wasm_worker() &&
    emscripten_stack_get_base() - emscripten_stack_get_end() == 1024 * 1024};
  for(std::size_t i{}; i != sizeof(bytes); ++i) bytes[i] = static_cast<unsigned char>(i % 251);
  for(std::size_t i{}; i != sizeof(bytes); ++i) passed = passed && bytes[i] == static_cast<unsigned char>(i % 251);
  result.store(passed ? 42 : -1);
  emscripten_wasm_worker_post_function_v(EMSCRIPTEN_WASM_WORKER_ID_PARENT, finished);
}
}

auto main()->int {
  worker = emscripten_malloc_wasm_worker(1024 * 1024);
  if(worker == 0) return test_result(false, "Could not create Wasm Worker");
  emscripten_wasm_worker_post_function_v(worker, run_worker);
  return 0;
}
