#include <cstdint>
#include <wasm_simd128.h>
#include "test_result.h"

namespace {
[[gnu::noinline]] auto add_vectors(v128_t left, v128_t right)->v128_t {
  return wasm_i32x4_add(left, right);
}
}

auto main()->int {
  std::int32_t volatile input{7};
  auto const result{add_vectors(wasm_i32x4_make(input, -2, 100, 0), wasm_i32x4_make(3, 5, -50, 42))};
  bool const passed{
    wasm_i32x4_extract_lane(result, 0) == 10 &&
    wasm_i32x4_extract_lane(result, 1) == 3 &&
    wasm_i32x4_extract_lane(result, 2) == 50 &&
    wasm_i32x4_extract_lane(result, 3) == 42
  };
  return test_result(passed, passed ? "i32x4.add · all four lanes verified" : "SIMD lane result mismatch");
}
