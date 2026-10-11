#include "webgpu_feature_test.h"
#include <cstdint>

namespace {
auto run()->void {
  wgpu::TextureDescriptor descriptor{
    .usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst,
    .dimension = wgpu::TextureDimension::e2D, .size{4, 4, 1}, .format = wgpu::TextureFormat::BC1RGBAUnorm
  };
  auto texture{gpu_test::device.CreateTexture(&descriptor)};
  // A BC1 block: RGB565 green and black endpoints, all indices selecting green.
  std::uint8_t const block[]{0xe0, 0x07, 0, 0, 0, 0, 0, 0};
  wgpu::TexelCopyTextureInfo destination{.texture = texture};
  wgpu::TexelCopyBufferLayout layout{.bytesPerRow = 8, .rowsPerImage = 1};
  wgpu::Extent3D extent{4, 4, 1};
  gpu_test::device.GetQueue().WriteTexture(&destination, block, sizeof(block), &layout, &extent);
  auto output{gpu_test::sample(texture, wgpu::FilterMode::Nearest)};
  gpu_test::verify(output, 16, [](void const *data) {
    auto const *value{static_cast<float const *>(data)};
    return value[0] == 0 && value[1] == 1 && value[2] == 0 && value[3] == 1;
  }, "BC1 upload · sampled colour verified");
}
}
auto main()->int {
  return gpu_test::start(wgpu::FeatureName::TextureCompressionBC, run);
}
