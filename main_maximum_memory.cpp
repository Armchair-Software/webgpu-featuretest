#include <cstdio>
#include <emscripten/heap.h>
#include "test_result.h"

auto main()->int {
  // These are the Emscripten queries also used by memorystorm. Heap size is
  // linear memory currently allocated, not live malloc usage or physical RAM.
  auto const current{emscripten_get_heap_size()};
  auto const maximum{emscripten_get_heap_max()};
  char detail[192]{};
  std::snprintf(detail, sizeof(detail), "Heap %.0f MiB · reported max %.4f MiB (no growth)",
    static_cast<double>(current) / 1048576.0, static_cast<double>(maximum) / 1048576.0);
  bool const passed{current == 16 * 1024 * 1024 && maximum >= current};
  return test_result(passed, passed ? detail : "Initial heap size or reported maximum is invalid");
}
