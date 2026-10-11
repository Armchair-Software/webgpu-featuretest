#pragma once

#include <cstddef>
#include <optional>
#include <webgpu/webgpu_cpp.h>

// Common setup and readback; pass nullopt for core WebGPU without extensions.
namespace gpu_test {
extern wgpu::Device device;
auto start(std::optional<wgpu::FeatureName> feature, void (*run)(),
           void (*report)(bool, char const *) = nullptr)->int;
auto buffer(std::size_t size, wgpu::BufferUsage usage)->wgpu::Buffer;
auto shader(char const *source)->wgpu::ShaderModule;
auto submit(wgpu::CommandEncoder const &encoder)->void;
auto verify(wgpu::Buffer const &source, std::size_t size, bool (*check)(void const *), char const *detail)->void;
auto sample(wgpu::Texture const &texture, wgpu::FilterMode filter)->wgpu::Buffer;
}
