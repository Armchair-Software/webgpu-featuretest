#include "webgpu_feature_test.h"
#include <cstdint>
#include <cstdlib>

namespace {
auto run()->void {
  auto module{gpu_test::shader(R"(
    enable dual_source_blending;
    @vertex fn vertex(@builtin(vertex_index) i: u32) -> @builtin(position) vec4f {
      let positions = array(vec2f(-1, -1), vec2f(3, -1), vec2f(-1, 3));
      return vec4f(positions[i], 0, 1);
    }
    struct Fragment {
      @location(0) @blend_src(0) colour: vec4f,
      @location(0) @blend_src(1) factor: vec4f,
    }
    @fragment fn fragment() -> Fragment {
      return Fragment(vec4f(0.25, 0.5, 1, 1), vec4f(0.5, 0.25, 0.75, 0.5));
    }
  )")};
  wgpu::BlendState blend{
    .color{.operation = wgpu::BlendOperation::Add, .srcFactor = wgpu::BlendFactor::Src1, .dstFactor = wgpu::BlendFactor::OneMinusSrc1},
    .alpha{.operation = wgpu::BlendOperation::Add, .srcFactor = wgpu::BlendFactor::One, .dstFactor = wgpu::BlendFactor::Zero}
  };
  wgpu::ColorTargetState colour{.format = wgpu::TextureFormat::RGBA8Unorm, .blend = &blend};
  wgpu::FragmentState fragment{.module = module, .entryPoint = "fragment", .targetCount = 1, .targets = &colour};
  wgpu::RenderPipelineDescriptor pipeline_descriptor{
    .vertex{.module = module, .entryPoint = "vertex"},
    .primitive{.topology = wgpu::PrimitiveTopology::TriangleList}, .fragment = &fragment
  };
  auto pipeline{gpu_test::device.CreateRenderPipeline(&pipeline_descriptor)};
  wgpu::TextureDescriptor texture_descriptor{
    .usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::CopySrc,
    .dimension = wgpu::TextureDimension::e2D, .size{1, 1, 1}, .format = wgpu::TextureFormat::RGBA8Unorm
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
    auto const *pixel{static_cast<std::uint8_t const *>(data)};
    // source0 * source1 + red * (1 - source1), allowing one UNORM rounding step.
    return std::abs(int{pixel[0]} - 159) <= 1 && std::abs(int{pixel[1]} - 32) <= 1 &&
      std::abs(int{pixel[2]} - 191) <= 1 && pixel[3] == 255;
  }, "Two fragment outputs · blending verified");
}
}
auto main()->int {
  return gpu_test::start(wgpu::FeatureName::DualSourceBlending, run);
}
