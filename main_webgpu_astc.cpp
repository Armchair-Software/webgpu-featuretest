#include "webgpu_feature_test.h"
#include <cstdint>
#include <cmath>

namespace {
auto run()->void {
  wgpu::TextureDescriptor descriptor{
    .usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst,
    .dimension = wgpu::TextureDimension::e2D, .size{4, 4, 1}, .format = wgpu::TextureFormat::ASTC4x4Unorm
  };
  auto texture{gpu_test::device.CreateTexture(&descriptor)};
  // ASTC LDR void-extent block, constant RGBA16 UNORM green with full alpha.
  // https://registry.khronos.org/DataFormat/specs/1.4/dataformat.1.4.html#astc-void-extent-blocks
  std::uint8_t const block[]{0xfc, 0xfd, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                             0, 0, 0xff, 0xff, 0, 0, 0xff, 0xff};
  wgpu::TexelCopyTextureInfo destination{.texture = texture};
  wgpu::TexelCopyBufferLayout layout{.bytesPerRow = 16, .rowsPerImage = 1};
  wgpu::Extent3D extent{4, 4, 1};
  gpu_test::device.GetQueue().WriteTexture(&destination, block, sizeof(block), &layout, &extent);
  auto output{gpu_test::sample(texture, wgpu::FilterMode::Nearest)};
  gpu_test::verify(output, 16, [](void const *data) {
    auto const *value{static_cast<float const *>(data)};
    return value[0] == 0 && std::abs(value[1] - 1.0F) < 0.001F && value[2] == 0 && value[3] == 1;
  }, "ASTC sampled colour verified");
}
}
auto main()->int {
  return gpu_test::start(wgpu::FeatureName::TextureCompressionASTC, run);
}
