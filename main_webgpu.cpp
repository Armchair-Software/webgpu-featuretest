#include "test_result.h"

#include <cstdint>
#include <string>
#include <utility>
#include <webgpu/webgpu_cpp.h>

namespace {
wgpu::Instance instance;
wgpu::Surface surface;
wgpu::Adapter adapter;
wgpu::Device device;
wgpu::Buffer readback;
wgpu::TextureFormat format;
bool stopped{};
bool configured{};

auto cleanup()->void {
  stopped = true;
  if(configured) surface.Unconfigure();
  configured = false;
  if(readback) readback.Destroy();
  readback = nullptr;
  if(device) device.Destroy();
  device = nullptr;
  surface = nullptr;
  adapter = nullptr;
  instance = nullptr;
}

auto report(bool passed, char const *detail)->void {
  if(stopped) return;
  cleanup();
  test_result(passed, detail);
}

auto fail(char const *stage, wgpu::StringView message)->void {
  std::string detail{stage};
  if(message.data && message.length) {
    detail += ": ";
    detail.append(message.data, message.length);
  }
  report(false, detail.c_str());
}

auto render()->void {
  wgpu::SurfaceConfiguration configuration{
    .device{device}, .format = format,
    .usage{wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::CopySrc},
    .width = 128, .height = 28, .presentMode = wgpu::PresentMode::Fifo
  };
  surface.Configure(&configuration);
  configured = true;
  wgpu::SurfaceTexture frame{};
  surface.GetCurrentTexture(&frame);
  if(!frame.texture) {
    report(false, "WebGPU canvas texture unavailable");
    return;
  }
  wgpu::BufferDescriptor buffer_descriptor{
    .usage{wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::MapRead}, .size = 256
  };
  readback = device.CreateBuffer(&buffer_descriptor);
  auto encoder{device.CreateCommandEncoder()};
  wgpu::RenderPassColorAttachment attachment{
    .view{frame.texture.CreateView()}, .loadOp = wgpu::LoadOp::Clear, .storeOp = wgpu::StoreOp::Store,
    .clearValue{22.0 / 255.0, 130.0 / 255.0, 73.0 / 255.0, 1.0}
  };
  wgpu::RenderPassDescriptor pass_descriptor{.colorAttachmentCount = 1, .colorAttachments = &attachment};
  auto pass{encoder.BeginRenderPass(&pass_descriptor)};
  pass.End();
  wgpu::TexelCopyTextureInfo source{.texture{frame.texture}, .aspect = wgpu::TextureAspect::All};
  wgpu::TexelCopyBufferInfo destination{.layout{.bytesPerRow = 256, .rowsPerImage = 1}, .buffer{readback}};
  wgpu::Extent3D extent{1, 1, 1};
  encoder.CopyTextureToBuffer(&source, &destination, &extent);
  auto commands{encoder.Finish()};
  device.GetQueue().Submit(1, &commands);
  readback.MapAsync(wgpu::MapMode::Read, 0, 256, wgpu::CallbackMode::AllowSpontaneous,
    [](wgpu::MapAsyncStatus status, wgpu::StringView message) {
      if(stopped) return;
      if(status != wgpu::MapAsyncStatus::Success) {
        fail("GPU readback failed", message);
        return;
      }
      auto const *pixel{static_cast<std::uint8_t const *>(readback.GetConstMappedRange(0, 4))};
      auto const bgra{format == wgpu::TextureFormat::BGRA8Unorm};
      bool const passed{pixel && pixel[bgra ? 2 : 0] == 22 && pixel[1] == 130 && pixel[bgra ? 0 : 2] == 73 && pixel[3] == 255};
      readback.Unmap();
      readback.Destroy();
      if(passed) EM_ASM({ document.getElementById('canvas').hidden = false; });
      report(passed, passed ? "Canvas clear · GPU readback verified" : "GPU pixel did not match #168249");
    });
}
} // namespace

extern "C" EMSCRIPTEN_KEEPALIVE auto cleanup_test()->void {
  cleanup();
}

auto main()->int {
  instance = wgpu::CreateInstance();
  wgpu::EmscriptenSurfaceSourceCanvasHTMLSelector canvas;
  canvas.selector = "#canvas";
  wgpu::SurfaceDescriptor descriptor{.nextInChain = &canvas};
  surface = instance.CreateSurface(&descriptor);
  wgpu::RequestAdapterOptions options{.compatibleSurface{surface}};
  instance.RequestAdapter(&options, wgpu::CallbackMode::AllowSpontaneous,
    [](wgpu::RequestAdapterStatus status, wgpu::Adapter result, wgpu::StringView message) {
      if(stopped) return;
      if(status != wgpu::RequestAdapterStatus::Success) {
        fail("WebGPU adapter unavailable", message);
        return;
      }
      adapter = std::move(result);
      wgpu::SurfaceCapabilities capabilities;
      surface.GetCapabilities(adapter, &capabilities);
      format = wgpu::TextureFormat::Undefined;
      for(std::size_t index{0}; index < capabilities.formatCount; ++index) {
        if(capabilities.formats[index] == wgpu::TextureFormat::BGRA8Unorm || capabilities.formats[index] == wgpu::TextureFormat::RGBA8Unorm) {
          format = capabilities.formats[index];
          break;
        }
      }
      if(format == wgpu::TextureFormat::Undefined) {
        report(false, "No supported 8-bit canvas format");
        return;
      }
      wgpu::DeviceDescriptor device_descriptor;
      device_descriptor.SetUncapturedErrorCallback([](wgpu::Device const &, wgpu::ErrorType, wgpu::StringView error) {
        fail("WebGPU error", error);
      });
      device_descriptor.SetDeviceLostCallback(wgpu::CallbackMode::AllowSpontaneous,
        [](wgpu::Device const &, wgpu::DeviceLostReason reason, wgpu::StringView error) {
          if(reason != wgpu::DeviceLostReason::Destroyed) fail("WebGPU device lost", error);
        });
      adapter.RequestDevice(&device_descriptor, wgpu::CallbackMode::AllowSpontaneous,
        [](wgpu::RequestDeviceStatus device_status, wgpu::Device result_device, wgpu::StringView error) {
          if(stopped) {
            if(result_device) result_device.Destroy();
            return;
          }
          if(device_status != wgpu::RequestDeviceStatus::Success) {
            fail("WebGPU device creation failed", error);
            return;
          }
          device = std::move(result_device);
          render();
        });
    });
  return 0;
}
