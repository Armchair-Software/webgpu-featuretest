#include "webgpu_feature_test.h"
#include <cstdint>

namespace {
auto run()->void {
  wgpu::TextureDescriptor descriptor{
    .usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::CopySrc,
    .dimension = wgpu::TextureDimension::e2D, .size{1, 1, 1}, .format = wgpu::TextureFormat::Depth32FloatStencil8
  };
  auto texture{gpu_test::device.CreateTexture(&descriptor)};
  wgpu::RenderPassDepthStencilAttachment attachment{
    .view = texture.CreateView(), .depthLoadOp = wgpu::LoadOp::Clear, .depthStoreOp = wgpu::StoreOp::Store,
    .depthClearValue = 0.25F, .stencilLoadOp = wgpu::LoadOp::Clear, .stencilStoreOp = wgpu::StoreOp::Store,
    .stencilClearValue = 7
  };
  wgpu::RenderPassDescriptor pass_descriptor{.colorAttachmentCount = 0, .depthStencilAttachment = &attachment};
  auto encoder{gpu_test::device.CreateCommandEncoder()};
  auto pass{encoder.BeginRenderPass(&pass_descriptor)};
  pass.End();
  auto output{gpu_test::buffer(512, wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::CopySrc)};
  wgpu::TexelCopyTextureInfo source{.texture = texture, .aspect = wgpu::TextureAspect::DepthOnly};
  wgpu::TexelCopyBufferInfo destination{.layout{.bytesPerRow = 256, .rowsPerImage = 1}, .buffer = output};
  wgpu::Extent3D extent{1, 1, 1};
  encoder.CopyTextureToBuffer(&source, &destination, &extent);
  source.aspect = wgpu::TextureAspect::StencilOnly;
  destination.layout.offset = 256;
  encoder.CopyTextureToBuffer(&source, &destination, &extent);
  gpu_test::submit(encoder);
  gpu_test::verify(output, 512, [](void const *data) {
    return *static_cast<float const *>(data) == 0.25F && static_cast<std::uint8_t const *>(data)[256] == 7;
  }, "Depth + stencil clear/readback verified");
}
}
auto main()->int {
  return gpu_test::start(wgpu::FeatureName::Depth32FloatStencil8, run);
}
