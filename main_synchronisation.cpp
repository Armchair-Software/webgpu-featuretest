#include <mutex>
#include <emscripten/wasm_worker.h>
#include "emscripten_sync.h"
#include "test_result.h"

namespace {
emscripten_sync::mutex mutex;
emscripten_wasm_worker_t workers[2]{};
unsigned int counter{};
unsigned int completed{};
bool held_lock_verified{};

auto finished()->void {
  if(++completed != 2) return;
  // The browser thread never blocks on a worker-held lock.
  bool const acquired{mutex.try_lock()};
  bool const passed{acquired && counter == 2000 && held_lock_verified};
  if(acquired) mutex.unlock();
  emscripten_terminate_all_wasm_workers();
  test_result(passed, passed ? "Two workers · locked counter + try_lock verified" : "Worker synchronization check failed");
}

auto increment()->void {
  for(unsigned int i{}; i != 1000; ++i) {
    std::lock_guard const guard{mutex};
    ++counter;
  }
  emscripten_wasm_worker_post_function_v(EMSCRIPTEN_WASM_WORKER_ID_PARENT, finished);
}

auto begin_increments(int rejected)->void {
  held_lock_verified = rejected != 0;
  mutex.unlock();
  for(auto const worker : workers) emscripten_wasm_worker_post_function_v(worker, increment);
}

auto check_held_lock()->void {
  bool const acquired{mutex.try_lock()};
  if(acquired) mutex.unlock();
  emscripten_wasm_worker_post_function_vi(EMSCRIPTEN_WASM_WORKER_ID_PARENT, begin_increments, acquired ? 0 : 1);
}
}

auto main()->int {
  for(auto &worker : workers) {
    worker = emscripten_malloc_wasm_worker(1024 * 1024);
    if(worker == 0) {
      emscripten_terminate_all_wasm_workers();
      return test_result(false, "Could not create synchronization workers");
    }
  }
  mutex.lock(); // No workers are using the lock yet.
  emscripten_wasm_worker_post_function_v(workers[0], check_held_lock);
  return 0;
}
