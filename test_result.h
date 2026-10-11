#pragma once

#include <emscripten.h>

static_assert(sizeof(void*) * 8 == TEST_POINTER_BITS);

inline auto test_result(bool passed, char const *detail)->int {
  EM_ASM({ window.report_result($0 !== 0, UTF8ToString($1)); }, passed, detail);
  return passed ? 0 : 1;
}
