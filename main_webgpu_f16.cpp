#include "webgpu_feature_test.h"

namespace {
auto run()->void {
  auto input{gpu_test::buffer(4, wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopyDst)};
  auto output{gpu_test::buffer(4, wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopySrc)};
  float const value{1.5F};
  gpu_test::device.GetQueue().WriteBuffer(input, 0, &value, sizeof(value));
  auto module{gpu_test::shader(R"(
    enable f16;
    @group(0) @binding(0) var<storage, read> input: f32;
    @group(0) @binding(1) var<storage, read_write> output: f32;
    @compute @workgroup_size(1) fn main() {
      output = f32(f16(input) * 2.0h + 0.5h);
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
  gpu_test::verify(output, 4, [](void const *data) { return *static_cast<float const *>(data) == 3.5F; },
    "f16 shader arithmetic verified");
}
}
auto main()->int {
  return gpu_test::start(wgpu::FeatureName::ShaderF16, run);
}
