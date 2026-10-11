#include "webgpu_feature_test.h"
#include <cmath>

namespace {
auto run()->void {
  wgpu::TextureDescriptor descriptor{
    .usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst,
    .dimension = wgpu::TextureDimension::e2D, .size{2, 1, 1}, .format = wgpu::TextureFormat::RGBA32Float
  };
  auto texture{gpu_test::device.CreateTexture(&descriptor)};
  float const pixels[]{0, 0, 0, 1, 1, 1, 1, 1};
  wgpu::TexelCopyTextureInfo destination{.texture = texture};
  wgpu::TexelCopyBufferLayout layout{.bytesPerRow = 32, .rowsPerImage = 1};
  wgpu::Extent3D extent{2, 1, 1};
  gpu_test::device.GetQueue().WriteTexture(&destination, pixels, sizeof(pixels), &layout, &extent);
  auto output{gpu_test::sample(texture, wgpu::FilterMode::Linear)};
  gpu_test::verify(output, 16, [](void const *data) {
    auto const *value{static_cast<float const *>(data)};
    return std::abs(value[0] - 0.5F) < 0.001F && std::abs(value[1] - 0.5F) < 0.001F &&
      std::abs(value[2] - 0.5F) < 0.001F && value[3] == 1;
  }, "RGBA32Float · linear filtering verified");
}
}
auto main()->int {
  return gpu_test::start(wgpu::FeatureName::Float32Filterable, run);
}
