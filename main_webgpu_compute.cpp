#include "webgpu_feature_test.h"
#include <cstdint>

namespace {
struct values {
  float floating;
  std::uint32_t integer;
};

auto run()->void {
  auto input{gpu_test::buffer(sizeof(values), wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopyDst)};
  auto output{gpu_test::buffer(sizeof(values), wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopySrc)};
  values const initial{1.5F, 17};
  gpu_test::device.GetQueue().WriteBuffer(input, 0, &initial, sizeof(initial));
  auto module{gpu_test::shader(R"(
    struct Values { floating: f32, integer: u32 }
    @group(0) @binding(0) var<storage, read> input: Values;
    @group(0) @binding(1) var<storage, read_write> output: Values;
    @compute @workgroup_size(1) fn main() {
      output.floating = input.floating * 2.0 + 0.5;
      output.integer = input.integer * 2u + 8u;
    }
  )")};
  wgpu::ComputePipelineDescriptor descriptor{.compute{.module = module, .entryPoint = "main"}};
  auto pipeline{gpu_test::device.CreateComputePipeline(&descriptor)};
  wgpu::BindGroupEntry entries[]{{.binding = 0, .buffer = input}, {.binding = 1, .buffer = output}};
  wgpu::BindGroupDescriptor group_descriptor{.layout = pipeline.GetBindGroupLayout(0), .entryCount = 2, .entries = entries};
  auto group{gpu_test::device.CreateBindGroup(&group_descriptor)};
  auto encoder{gpu_test::device.CreateCommandEncoder()};
  auto pass{encoder.BeginComputePass()};
  pass.SetPipeline(pipeline);
  pass.SetBindGroup(0, group);
  pass.DispatchWorkgroups(1);
  pass.End();
  gpu_test::submit(encoder);
  gpu_test::verify(output, sizeof(values), [](void const *data) {
    auto const &value{*static_cast<values const *>(data)};
    return value.floating == 3.5F && value.integer == 42;
  }, "f32 + u32 compute results verified");
}
}
auto main()->int {
  return gpu_test::start(std::nullopt, run);
}
