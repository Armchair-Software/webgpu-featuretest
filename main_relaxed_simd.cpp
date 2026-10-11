#include <wasm_simd128.h>
#include "test_result.h"

namespace {
[[gnu::noinline]] auto multiply_add(v128_t left, v128_t right, v128_t addend)->v128_t {
  return wasm_f32x4_relaxed_madd(left, right, addend);
}
}

auto main()->int {
  float volatile input{7.0f};
  // Exactly representable values give the same answer with fused or unfused
  // evaluation, both of which are permitted by relaxed SIMD.
  auto const result{multiply_add(
    wasm_f32x4_make(input, -2.0f, 0.5f, 0.0f),
    wasm_f32x4_make(2.0f, 3.0f, 8.0f, 10.0f),
    wasm_f32x4_make(1.0f, 2.0f, 0.5f, -1.0f)
  )};
  bool const passed{
    wasm_f32x4_extract_lane(result, 0) == 15.0f &&
    wasm_f32x4_extract_lane(result, 1) == -4.0f &&
    wasm_f32x4_extract_lane(result, 2) == 4.5f &&
    wasm_f32x4_extract_lane(result, 3) == -1.0f
  };
  return test_result(passed, passed ? "Relaxed multiply-add · four lanes verified" : "Relaxed SIMD lane result mismatch");
}
