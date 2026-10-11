#include <cstdint>
#include <emscripten.h>

static_assert(sizeof(void*) * 8 == TEST_POINTER_BITS);

auto main()->int {
  // Volatile input keeps an actual load and computation in optimised builds.
  std::uint32_t volatile input{21};
  std::uint32_t const result{input * 2};
  bool const passed{result == 42};
  EM_ASM({
    window.report_result($0 !== 0, 'C++ main ran · ' + $1 + '-bit pointers · 21 × 2 = ' + $2);
  }, passed, TEST_POINTER_BITS, result);
  return passed ? 0 : 1;
}
