#include <cerrno>
#include <mutex>
#include <pthread.h>
#include "test_result.h"

namespace {
pthread_t thread;
std::mutex mutex;
int result{};

auto run_thread(void*)->void* {
  std::lock_guard const guard{mutex};
  result = 42;
  return &result;
}

auto poll_join(void*)->void {
  void *returned{};
  int const status{pthread_tryjoin_np(thread, &returned)};
  if(status == EBUSY) {
    emscripten_async_call(poll_join, nullptr, 10);
    return;
  }
  bool const passed{status == 0 && returned == &result && result == 42};
  int const exit_code{test_result(passed, passed ? "Pthread create / mutex / join verified" : "Pthread join or result check failed")};
  emscripten_force_exit(exit_code); // Release the Emscripten pthread worker pool.
}
}

auto main()->int {
  if(pthread_create(&thread, nullptr, run_thread, nullptr) != 0) {
    return test_result(false, "Could not create pthread");
  }
  emscripten_async_call(poll_join, nullptr, 0);
  emscripten_exit_with_live_runtime();
}
