#include "webgpu_feature_test.h"
#include <cstdint>
#include <limits>

namespace {
auto run()->void {
  wgpu::QuerySetDescriptor descriptor{.type = wgpu::QueryType::Timestamp, .count = 2};
  auto queries{gpu_test::device.CreateQuerySet(&descriptor)};
  wgpu::PassTimestampWrites timestamps{.querySet = queries, .beginningOfPassWriteIndex = 0, .endOfPassWriteIndex = 1};
  wgpu::ComputePassDescriptor pass_descriptor{.timestampWrites = &timestamps};
  auto encoder{gpu_test::device.CreateCommandEncoder()};
  auto pass{encoder.BeginComputePass(&pass_descriptor)};
  pass.End();
  auto output{gpu_test::buffer(16, wgpu::BufferUsage::QueryResolve | wgpu::BufferUsage::CopySrc | wgpu::BufferUsage::CopyDst)};
  std::uint64_t const sentinel{std::numeric_limits<std::uint64_t>::max()};
  std::uint64_t const initial[]{sentinel, sentinel};
  gpu_test::device.GetQueue().WriteBuffer(output, 0, initial, sizeof(initial));
  encoder.ResolveQuerySet(queries, 0, 2, output, 0);
  gpu_test::submit(encoder);
  gpu_test::verify(output, 16, [](void const *data) {
    auto const *values{static_cast<std::uint64_t const *>(data)};
    // Quantization can make timestamps equal or zero; both must replace the sentinels.
    auto const unwritten{std::numeric_limits<std::uint64_t>::max()};
    return values[0] != unwritten && values[1] != unwritten && values[1] >= values[0];
  }, "GPU timestamps written and resolved");
}
}
auto main()->int {
  return gpu_test::start(wgpu::FeatureName::TimestampQuery, run);
}
