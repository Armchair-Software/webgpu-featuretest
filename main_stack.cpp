#include <cstddef>
#include <cstdio>
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
  auto const size{emscripten_stack_get_base() - emscripten_stack_get_end()};
  auto const available{emscripten_stack_get_free()};
  bool const configured{size == 5 * 1024 * 1024};
  if(!configured) return test_result(false, "Stack bounds differ from configured 5 MiB");
  auto const before{emscripten_stack_get_current()};
  bool const passed{exercise_stack() && emscripten_stack_get_current() == before};
  char detail[160]{};
  std::snprintf(detail, sizeof(detail), "Stack %.0f KiB · free %.2f KiB · 1 MiB verified",
    static_cast<double>(size) / 1024.0, static_cast<double>(available) / 1024.0);
  return test_result(passed, passed ? detail : "Stack data or restored stack pointer differs");
}
