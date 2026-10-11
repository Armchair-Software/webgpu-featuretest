#include "webgpu_feature_test.h"
#include "test_result.h"
#include <string>
#include <utility>
#ifdef __EMSCRIPTEN_WASM_WORKERS__
#include <emscripten/wasm_worker.h>
#endif

namespace gpu_test {
wgpu::Device device;
namespace {
wgpu::Instance instance;
wgpu::Adapter adapter;
std::optional<wgpu::FeatureName> required_feature;
void (*run_test)();
void (*report_result)(bool, char const *);
bool stopped{};

auto cleanup()->void {
  stopped = true;
  if(device) device.Destroy();
  device = nullptr;
  adapter = nullptr;
  instance = nullptr;
}

auto report(bool passed, char const *detail)->void {
  if(stopped) return;
  cleanup();
  if(report_result) report_result(passed, detail);
  else test_result(passed, detail);
}

auto fail(char const *stage, wgpu::StringView message)->void {
  std::string detail{stage};
  if(message.data && message.length) {
    detail += ": ";
    detail.append(message.data, message.length);
  }
  report(false, detail.c_str());
}
}

auto start(std::optional<wgpu::FeatureName> feature, void (*run)(), void (*report_callback)(bool, char const *))->int {
  report_result = report_callback;
  required_feature = feature;
  run_test = run;
  instance = wgpu::CreateInstance();
  instance.RequestAdapter(nullptr, wgpu::CallbackMode::AllowSpontaneous,
    [](wgpu::RequestAdapterStatus status, wgpu::Adapter result, wgpu::StringView message) {
      if(stopped) return;
      if(status != wgpu::RequestAdapterStatus::Success) {
        fail("Adapter unavailable", message);
        return;
      }
      adapter = std::move(result);
      if(required_feature && !adapter.HasFeature(*required_feature)) {
        report(false, "Extension not supported by adapter");
        return;
      }
      wgpu::DeviceDescriptor descriptor;
      if(required_feature) {
        descriptor.requiredFeatureCount = 1;
        descriptor.requiredFeatures = &*required_feature;
      }
      descriptor.SetUncapturedErrorCallback([](wgpu::Device const &, wgpu::ErrorType, wgpu::StringView error) {
        fail("GPU error", error);
      });
      descriptor.SetDeviceLostCallback(wgpu::CallbackMode::AllowSpontaneous,
        [](wgpu::Device const &, wgpu::DeviceLostReason reason, wgpu::StringView error) {
          if(reason != wgpu::DeviceLostReason::Destroyed) fail("Device lost", error);
        });
      adapter.RequestDevice(&descriptor, wgpu::CallbackMode::AllowSpontaneous,
        [](wgpu::RequestDeviceStatus status, wgpu::Device result, wgpu::StringView error) {
          if(stopped) {
            if(result) result.Destroy();
            return;
          }
          if(status != wgpu::RequestDeviceStatus::Success) {
            fail("Device creation failed", error);
            return;
          }
          device = std::move(result);
          device.PushErrorScope(wgpu::ErrorFilter::Validation);
          run_test();
        });
    });
  return 0;
}

auto buffer(std::size_t size, wgpu::BufferUsage usage)->wgpu::Buffer {
  wgpu::BufferDescriptor descriptor{.usage = usage, .size = size};
  return device.CreateBuffer(&descriptor);
}

auto shader(char const *source)->wgpu::ShaderModule {
  wgpu::ShaderSourceWGSL wgsl;
  wgsl.code = source;
  wgpu::ShaderModuleDescriptor descriptor{.nextInChain = &wgsl};
  return device.CreateShaderModule(&descriptor);
}

auto submit(wgpu::CommandEncoder const &encoder)->void {
  auto commands{encoder.Finish()};
  device.GetQueue().Submit(1, &commands);
}

auto verify(wgpu::Buffer const &source, std::size_t size, bool (*check)(void const *), char const *detail)->void {
  auto readback{buffer(size, wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::MapRead)};
  auto encoder{device.CreateCommandEncoder()};
  encoder.CopyBufferToBuffer(source, 0, readback, 0, size);
  submit(encoder);
  readback.MapAsync(wgpu::MapMode::Read, 0, size, wgpu::CallbackMode::AllowSpontaneous,
    [readback, size, check, detail](wgpu::MapAsyncStatus status, wgpu::StringView message) {
      if(stopped) return;
      if(status != wgpu::MapAsyncStatus::Success) {
        fail("Readback failed", message);
        return;
      }
      auto const *data{readback.GetConstMappedRange(0, size)};
      bool const passed{data && check(data)};
      readback.Unmap();
      readback.Destroy();
      device.PopErrorScope(wgpu::CallbackMode::AllowSpontaneous,
        [passed, detail](wgpu::PopErrorScopeStatus scope_status, wgpu::ErrorType type, wgpu::StringView error) {
          if(scope_status != wgpu::PopErrorScopeStatus::Success || type != wgpu::ErrorType::NoError) {
            fail("GPU validation failed", error);
          } else {
            report(passed, passed ? detail : "GPU readback mismatch");
          }
        });
    });
}

auto sample(wgpu::Texture const &texture, wgpu::FilterMode filter)->wgpu::Buffer {
  auto module{shader(R"(
    @group(0) @binding(0) var image: texture_2d<f32>;
    @group(0) @binding(1) var image_sampler: sampler;
    @group(0) @binding(2) var<storage, read_write> result: vec4f;
    @compute @workgroup_size(1) fn main() {
      result = textureSampleLevel(image, image_sampler, vec2f(0.5), 0.0);
    }
  )")};
  wgpu::ComputePipelineDescriptor pipeline_descriptor{.compute{.module = module, .entryPoint = "main"}};
  auto pipeline{device.CreateComputePipeline(&pipeline_descriptor)};
  auto output{buffer(16, wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopySrc)};
  wgpu::SamplerDescriptor sampler_descriptor{.magFilter = filter, .minFilter = filter};
  auto sampler{device.CreateSampler(&sampler_descriptor)};
  wgpu::BindGroupEntry entries[]{
    {.binding = 0, .textureView = texture.CreateView()},
    {.binding = 1, .sampler = sampler},
    {.binding = 2, .buffer = output}
  };
  wgpu::BindGroupDescriptor group_descriptor{.layout = pipeline.GetBindGroupLayout(0), .entryCount = 3, .entries = entries};
  auto group{device.CreateBindGroup(&group_descriptor)};
  auto encoder{device.CreateCommandEncoder()};
  auto pass{encoder.BeginComputePass()};
  pass.SetPipeline(pipeline);
  pass.SetBindGroup(0, group);
  pass.DispatchWorkgroups(1);
  pass.End();
  submit(encoder);
  return output;
}
}

// Called before the runner is removed, including on timeout or page refresh.
extern "C" EMSCRIPTEN_KEEPALIVE auto cleanup_test()->void {
#ifdef __EMSCRIPTEN_WASM_WORKERS__
  if(!emscripten_current_thread_is_wasm_worker()) {
    emscripten_terminate_all_wasm_workers();
    return;
  }
#endif
  gpu_test::cleanup();
}
