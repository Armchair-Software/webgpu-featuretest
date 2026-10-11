#include <cstddef>
#include <cstring>
#include "test_result.h"

namespace {
[[gnu::noinline]] auto fill_bytes(void *destination, std::size_t size)->void {
  std::memset(destination, 0x5a, size);
}

[[gnu::noinline]] auto copy_bytes(void *destination, void const *source, std::size_t size)->void {
  std::memcpy(destination, source, size);
}
}

auto main()->int {
  unsigned char source[257]{};
  unsigned char destination[259]{};
  fill_bytes(source, sizeof(source));
  copy_bytes(destination + 1, source, sizeof(source));
  bool passed{destination[0] == 0 && destination[258] == 0};
  for(std::size_t i{1}; i != 258; ++i) passed = passed && destination[i] == 0x5a;
  return test_result(passed, passed ? "Memory fill / copy · 257 bytes + boundary guards" : "Copied bytes or boundary guards differ");
}
