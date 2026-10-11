#include <atomic>
#include <cstdint>
#include "test_result.h"

auto main()->int {
  static_assert(std::atomic<std::int32_t>::is_always_lock_free);
  std::atomic<std::int32_t> value{7};
  bool const added{value.fetch_add(5) == 7};
  std::int32_t expected{12};
  bool const exchanged{value.compare_exchange_strong(expected, 42)};
  // Verify that JS sees the same shared memory and can update the C++ atomic.
  bool const shared{EM_ASM_INT({
    return HEAP32.buffer instanceof SharedArrayBuffer &&
      Atomics.compareExchange(HEAP32, Number($0) / 4, 42, 99) === 42;
  }, &value) != 0};
  bool const passed{added && exchanged && shared && value.load() == 99};
  return test_result(passed, passed ? "Shared memory · C++ / JS atomics verified" : "Shared memory or atomic operation failed");
}
