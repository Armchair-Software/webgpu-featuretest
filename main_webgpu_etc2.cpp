#include "webgpu_feature_test.h"
#include <cstdint>
#include <cmath>

namespace {
auto run()->void {
  wgpu::TextureDescriptor descriptor{
    .usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst,
    .dimension = wgpu::TextureDimension::e2D, .size{4, 4, 1}, .format = wgpu::TextureFormat::ETC2RGB8Unorm
  };
  auto texture{gpu_test::device.CreateTexture(&descriptor)};
  // ETC2 individual mode: white base colours, table 0, selector 0 (+2, clamped).
  // https://registry.khronos.org/DataFormat/specs/1.4/dataformat.1.4.html#etc2
  std::uint8_t const block[]{0xff, 0xff, 0xff, 0, 0, 0, 0, 0};
  wgpu::TexelCopyTextureInfo destination{.texture = texture};
  wgpu::TexelCopyBufferLayout layout{.bytesPerRow = 8, .rowsPerImage = 1};
  wgpu::Extent3D extent{4, 4, 1};
  gpu_test::device.GetQueue().WriteTexture(&destination, block, sizeof(block), &layout, &extent);
  auto output{gpu_test::sample(texture, wgpu::FilterMode::Nearest)};
  gpu_test::verify(output, 16, [](void const *data) {
    auto const *value{static_cast<float const *>(data)};
    return std::abs(value[0] - 1.0F) < 0.001F && std::abs(value[1] - 1.0F) < 0.001F &&
      std::abs(value[2] - 1.0F) < 0.001F && value[3] == 1;
  }, "ETC2 sampled colour verified");
}
}
auto main()->int {
  return gpu_test::start(wgpu::FeatureName::TextureCompressionETC2, run);
}
