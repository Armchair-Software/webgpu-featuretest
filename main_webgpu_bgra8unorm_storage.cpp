#include "webgpu_feature_test.h"
#include <cstdint>

namespace {
auto run()->void {
  auto module{gpu_test::shader(R"(
    @group(0) @binding(0) var image: texture_storage_2d<bgra8unorm, write>;
    @compute @workgroup_size(1) fn main() {
      textureStore(image, vec2i(0), vec4f(22.0 / 255.0, 130.0 / 255.0, 73.0 / 255.0, 1));
    }
  )")};
  wgpu::ComputePipelineDescriptor pipeline_descriptor{.compute{.module = module, .entryPoint = "main"}};
  auto pipeline{gpu_test::device.CreateComputePipeline(&pipeline_descriptor)};
  wgpu::TextureDescriptor texture_descriptor{
    .usage = wgpu::TextureUsage::StorageBinding | wgpu::TextureUsage::CopySrc,
    .dimension = wgpu::TextureDimension::e2D, .size{1, 1, 1}, .format = wgpu::TextureFormat::BGRA8Unorm
  };
  auto texture{gpu_test::device.CreateTexture(&texture_descriptor)};
  wgpu::BindGroupEntry entry{.binding = 0, .textureView = texture.CreateView()};
  wgpu::BindGroupDescriptor group_descriptor{.layout = pipeline.GetBindGroupLayout(0), .entryCount = 1, .entries = &entry};
  auto group{gpu_test::device.CreateBindGroup(&group_descriptor)};
  auto encoder{gpu_test::device.CreateCommandEncoder()};
  auto pass{encoder.BeginComputePass()};
  pass.SetPipeline(pipeline);
  pass.SetBindGroup(0, group);
  pass.DispatchWorkgroups(1);
  pass.End();
  auto output{gpu_test::buffer(256, wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::CopySrc)};
  wgpu::TexelCopyTextureInfo source{.texture = texture};
  wgpu::TexelCopyBufferInfo destination{.layout{.bytesPerRow = 256, .rowsPerImage = 1}, .buffer = output};
  wgpu::Extent3D extent{1, 1, 1};
  encoder.CopyTextureToBuffer(&source, &destination, &extent);
  gpu_test::submit(encoder);
  gpu_test::verify(output, 256, [](void const *data) {
    auto const *pixel{static_cast<std::uint8_t const *>(data)};
    return pixel[0] == 73 && pixel[1] == 130 && pixel[2] == 22 && pixel[3] == 255;
  }, "BGRA storage write + readback verified");
}
}
auto main()->int {
  return gpu_test::start(wgpu::FeatureName::BGRA8UnormStorage, run);
}
