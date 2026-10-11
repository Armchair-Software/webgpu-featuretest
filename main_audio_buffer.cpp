#include "test_result.h"

// Offline rendering tests the basic Web Audio graph without autoplay or speakers.
auto main()->int {
  EM_ASM({
    (async () => {
      const context = new OfflineAudioContext(1, 128, 48000);
      const buffer = context.createBuffer(1, 128, 48000);
      buffer.getChannelData(0).fill(0.25);
      const source = context.createBufferSource();
      source.buffer = buffer;
      source.connect(context.destination);
      source.start();
      const rendered = await context.startRendering();
      const samples = rendered.getChannelData(0);
      const passed = samples.length === 128 && samples.every(value => value === 0.25);
      source.disconnect();
      window.report_result(passed, passed ? '128 audio samples rendered and verified offline' : 'Rendered audio samples did not match');
    })().catch(error => window.report_result(false, String(error)));
  });
  return 0;
}
