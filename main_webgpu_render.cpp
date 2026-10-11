#include "webgpu_feature_test.h"
#include <cstdint>

namespace {
auto run()->void {
  auto module{gpu_test::shader(R"(
    struct Vertex { @builtin(position) position: vec4f }
    @vertex fn vertex(@builtin(vertex_index) i: u32) -> Vertex {
      let positions = array(vec2f(-1, -1), vec2f(3, -1), vec2f(-1, 3));
      return Vertex(vec4f(positions[i], 0, 1));
    }
    @fragment fn fragment() -> @location(0) vec4f {
      return vec4f(22.0 / 255.0, 130.0 / 255.0, 73.0 / 255.0, 1);
    }
  )")};
  wgpu::ColorTargetState colour{.format = wgpu::TextureFormat::RGBA8Unorm};
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
    return pixel[0] == 22 && pixel[1] == 130 && pixel[2] == 73 && pixel[3] == 255;
  }, "Vertex + fragment shaders verified");
}
}
auto main()->int {
  return gpu_test::start(std::nullopt, run);
}
