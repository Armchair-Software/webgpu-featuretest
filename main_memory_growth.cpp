#include <cstddef>
#include <cstdlib>
#include <emscripten/heap.h>
#include "test_result.h"

auto main()->int {
  auto *before{static_cast<unsigned char*>(std::malloc(16))};
  if(before == nullptr) return test_result(false, "Initial allocation failed");
  before[0] = 0x5a;
  std::size_t const initial{emscripten_get_heap_size()};
  // Allocating more than the entire initial heap forces growth, without a large stress test.
  auto *allocation{static_cast<unsigned char*>(std::malloc(initial))};
  if(allocation == nullptr) {
    std::free(before);
    return test_result(false, "Allocation requiring heap growth failed");
  }
  auto *volatile_bytes{static_cast<unsigned char volatile*>(allocation)};
  volatile_bytes[0] = 0x12;
  volatile_bytes[initial - 1] = 0x34;
  bool const passed{emscripten_get_heap_size() > initial && before[0] == 0x5a &&
    volatile_bytes[0] == 0x12 && volatile_bytes[initial - 1] == 0x34};
  std::free(allocation);
  std::free(before);
  return test_result(passed, passed ? "Heap grew · old data and new allocation verified" : "Heap failed to grow or memory contents differ");
}
