#include "webgpu_feature_test.h"
#include <cstdint>

namespace {
auto run()->void {
  auto module{gpu_test::shader(R"(
    @vertex fn vertex(@builtin(vertex_index) i: u32) -> @builtin(position) vec4f {
      let positions = array(vec2f(-1, -1), vec2f(3, -1), vec2f(-1, 3));
      return vec4f(positions[i], 0, 1);
    }
    @fragment fn fragment() -> @location(0) vec4f {
      return vec4f(0.25, 0.5, 1, 1);
    }
  )")};
  wgpu::ColorTargetState colour{.format = wgpu::TextureFormat::RG11B10Ufloat};
  wgpu::FragmentState fragment{.module = module, .entryPoint = "fragment", .targetCount = 1, .targets = &colour};
  wgpu::RenderPipelineDescriptor pipeline_descriptor{
    .vertex{.module = module, .entryPoint = "vertex"},
    .primitive{.topology = wgpu::PrimitiveTopology::TriangleList}, .fragment = &fragment
  };
  auto pipeline{gpu_test::device.CreateRenderPipeline(&pipeline_descriptor)};
  wgpu::TextureDescriptor texture_descriptor{
    .usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::CopySrc,
    .dimension = wgpu::TextureDimension::e2D, .size{1, 1, 1}, .format = wgpu::TextureFormat::RG11B10Ufloat
  };
  auto texture{gpu_test::device.CreateTexture(&texture_descriptor)};
  wgpu::RenderPassColorAttachment attachment{
    .view = texture.CreateView(), .loadOp = wgpu::LoadOp::Clear, .storeOp = wgpu::StoreOp::Store,
    .clearValue{1, 0, 0, 1}
  };
  wgpu::RenderPassDescriptor pass_descriptor{.colorAttachmentCount = 1, .colorAttachments = &attachment};
  auto encoder{gpu_test::device.CreateCommandEncoder()};
  auto pass{encoder.BeginRenderPass(&pass_descriptor)};
  pass.SetPipeline(pipeline);
  pass.Draw(3);
  pass.End();
  auto output{gpu_test::buffer(256, wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::CopySrc)};
  wgpu::TexelCopyTextureInfo source{.texture = texture};
  wgpu::TexelCopyBufferInfo destination{.layout{.bytesPerRow = 256, .rowsPerImage = 1}, .buffer = output};
  wgpu::Extent3D extent{1, 1, 1};
  encoder.CopyTextureToBuffer(&source, &destination, &extent);
  gpu_test::submit(encoder);
  gpu_test::verify(output, 256, [](void const *data) {
    // Packed unsigned floats: 5 exponent bits (bias 15), then 6/6/5 mantissa bits.
    std::uint32_t constexpr expected{(13U << 6) | (14U << 17) | (15U << 27)};
    return *static_cast<std::uint32_t const *>(data) == expected;
  }, "Packed float render + readback verified");
}
}
auto main()->int {
  return gpu_test::start(wgpu::FeatureName::RG11B10UfloatRenderable, run);
}
