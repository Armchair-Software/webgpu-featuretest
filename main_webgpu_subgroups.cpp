#include "webgpu_feature_test.h"
#include <cstdint>

namespace {
auto run()->void {
  auto module{gpu_test::shader(R"(
    enable subgroups;
    @group(0) @binding(0) var<storage, read_write> results: array<vec4u>;
    @compute @workgroup_size(128) fn main(
      @builtin(local_invocation_index) index: u32,
      @builtin(subgroup_size) size: u32
    ) {
      results[index] = vec4u(subgroupAdd(3u), subgroupAdd(1u), subgroupExclusiveAdd(3u), size);
    }
  )")};
  wgpu::ComputePipelineDescriptor pipeline_descriptor{.compute{.module = module, .entryPoint = "main"}};
  auto pipeline{gpu_test::device.CreateComputePipeline(&pipeline_descriptor)};
  auto output{gpu_test::buffer(2048, wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopySrc)};
  wgpu::BindGroupEntry entry{.binding = 0, .buffer = output};
  wgpu::BindGroupDescriptor group_descriptor{.layout = pipeline.GetBindGroupLayout(0), .entryCount = 1, .entries = &entry};
  auto group{gpu_test::device.CreateBindGroup(&group_descriptor)};
  auto encoder{gpu_test::device.CreateCommandEncoder()};
  auto pass{encoder.BeginComputePass()};
  pass.SetPipeline(pipeline);
  pass.SetBindGroup(0, group);
  pass.DispatchWorkgroups(1);
  pass.End();
  gpu_test::submit(encoder);
  gpu_test::verify(output, 2048, [](void const *data) {
    auto const *values{static_cast<std::uint32_t const *>(data)};
    // Count active lanes rather than assuming a subgroup size or lane assignment.
    for(unsigned int index{}; index < 128; ++index) {
      auto const sum{values[index * 4]};
      auto const count{values[index * 4 + 1]};
      auto const prefix{values[index * 4 + 2]};
      auto const size{values[index * 4 + 3]};
      if(size < 4 || size > 128 || (size & (size - 1)) != 0 ||
         count == 0 || count > size || sum != count * 3 || prefix >= sum || prefix % 3 != 0) return false;
    }
    return true;
  }, "Subgroup sums · 128 invocations verified");
}
}
auto main()->int {
  return gpu_test::start(wgpu::FeatureName::Subgroups, run);
}
