#include "test_result.h"

#include <atomic>
#include <cstdint>
#include <emscripten/webaudio.h>

namespace {
alignas(16) std::uint8_t worklet_stack[4096]{};
std::atomic<unsigned int> processed_blocks{};
EMSCRIPTEN_WEBAUDIO_T context{};
EMSCRIPTEN_WEBAUDIO_T node{};
double deadline{};

auto cleanup()->void {
  if(node) emscripten_destroy_web_audio_node(node);
  if(context) emscripten_destroy_audio_context(context);
}

auto check_processing(void *)->void {
  if(processed_blocks.load() > 0) {
    cleanup();
    test_result(true, "Wasm worklet processed silent audio on its own thread · 4 KiB stack");
  } else if(emscripten_get_now() < deadline) {
    emscripten_async_call(check_processing, nullptr, 50);
  } else {
    auto const state{emscripten_audio_context_state(context)};
    cleanup();
    if(state == AUDIO_CONTEXT_STATE_SUSPENDED) {
      EM_ASM({ window.report_blocked('Worklet created; audio context suspended (autoplay may require interaction)'); });
    } else {
      test_result(false, "Worklet created but no audio processed within 2 seconds");
    }
  }
}

auto process(int, AudioSampleFrame const *, int output_count, AudioSampleFrame *outputs,
             int, AudioParamFrame const *, void *)->bool {
  for(int output{0}; output < output_count; ++output) {
    auto &frame{outputs[output]};
    for(int sample{0}; sample < frame.numberOfChannels * frame.samplesPerChannel; ++sample) {
      frame.data[sample] = 0.0F;
    }
  }
  if(output_count > 0 && outputs[0].samplesPerChannel > 0 && emscripten_current_thread_is_audio_worklet()) {
    processed_blocks.fetch_add(1);
  }
  return true;
}

auto processor_created(EMSCRIPTEN_WEBAUDIO_T, bool success, void *)->void {
  if(!success) {
    cleanup();
    test_result(false, "Wasm audio processor registration failed");
    return;
  }
  int channels[]{1};
  EmscriptenAudioWorkletNodeCreateOptions options{};
  options.numberOfInputs = 0;
  options.numberOfOutputs = 1;
  options.outputChannelCounts = channels;
  node = emscripten_create_wasm_audio_worklet_node(context, "silent-test", &options, process, nullptr);
  if(!node) {
    cleanup();
    test_result(false, "Wasm audio node creation failed");
    return;
  }
  emscripten_audio_node_connect(node, context, 0, 0);
  emscripten_resume_audio_context_sync(context);
  deadline = emscripten_get_now() + 2000;
  check_processing(nullptr);
}

auto thread_started(EMSCRIPTEN_WEBAUDIO_T, bool success, void *)->void {
  if(!success) {
    cleanup();
    test_result(false, "Wasm audio worklet thread failed to start");
    return;
  }
  WebAudioWorkletProcessorCreateOptions options{};
  options.name = "silent-test";
  emscripten_create_wasm_audio_worklet_processor_async(context, &options, processor_created, nullptr);
}
} // namespace

auto main()->int {
  EmscriptenWebAudioCreateAttributes options{};
  options.latencyHint = "interactive";
  context = emscripten_create_audio_context(&options);
  if(!context) return test_result(false, "AudioContext creation failed");
  emscripten_start_wasm_audio_worklet_thread_async(context, worklet_stack, sizeof(worklet_stack), thread_started, nullptr);
  return 0;
}
