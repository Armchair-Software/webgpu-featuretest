#include <cstdio>
#include <cstring>
#include "test_result.h"

auto main()->int {
  auto *file{std::fopen("/fixture.txt", "rb")};
  if(file == nullptr) return test_result(false, "Preloaded fixture missing from virtual filesystem");
  char constexpr expected[]{"Browser feature test fixture.\n"};
  char buffer[sizeof(expected)]{};
  auto const size{std::fread(buffer, 1, sizeof(buffer), file)};
  bool const passed{size == sizeof(expected) - 1 && std::feof(file) &&
    std::memcmp(buffer, expected, size) == 0};
  std::fclose(file);
  return test_result(passed, passed ? "Preloaded file · C++ read verified" : "Preloaded file contents differ");
}
