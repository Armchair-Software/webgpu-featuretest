#include <cstddef>
#include <emscripten/stack.h>
#include "test_result.h"

namespace {
[[gnu::noinline]] auto exercise_stack()->bool {
  // Volatile storage prevents optimisation into a constant result or a smaller frame.
  unsigned char volatile bytes[1024 * 1024];
  for(std::size_t i{}; i != sizeof(bytes); ++i) bytes[i] = static_cast<unsigned char>(i % 251);
  for(std::size_t i{}; i != sizeof(bytes); ++i) {
    if(bytes[i] != static_cast<unsigned char>(i % 251)) return false;
  }
  return true;
}
}

auto main()->int {
  bool const configured{emscripten_stack_get_base() - emscripten_stack_get_end() == 5 * 1024 * 1024};
  if(!configured) return test_result(false, "Stack bounds differ from configured 5 MiB");
  auto const before{emscripten_stack_get_current()};
  bool const passed{exercise_stack() && emscripten_stack_get_current() == before};
  return test_result(passed, passed ? "5 MiB stack · 1 MiB local array verified" : "Stack data or restored stack pointer differs");
}
