#include "test_result.h"

#include <cstddef>
#include <cstdint>
#include <new>

static_assert(sizeof(std::size_t) * 8 == TEST_POINTER_BITS && sizeof(std::uintptr_t) * 8 == TEST_POINTER_BITS);

auto main()->int {
  // Volatile values keep heap accesses and arithmetic in optimised builds too.
  std::uint32_t volatile input{21};
  if(input * 2 != 42) return test_result(false, "Basic arithmetic failed");
  auto *values{new(std::nothrow) std::uint64_t volatile[2]};
  if(!values) return test_result(false, "Heap allocation failed");
  values[0] = 0xffff'ffffULL;
  values[1] = values[0] + 2;
  std::uint64_t const result{values[1]};
  delete[] values;
  if(result != 0x1'0000'0001ULL) return test_result(false, "Heap access / i64 arithmetic failed");

  bool const passed{EM_ASM_INT({
    return typeof $0 === 'bigint' && $0 === BigInt('4294967297');
  }, result) != 0};
  return test_result(passed, passed ? "Heap + i64 arithmetic / BigInt verified" : "BigInt interop failed");
}
