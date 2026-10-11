#include "test_result.h"

auto main()->int {
  bool const passed{EM_ASM_INT({
    const canvas = document.getElementById('canvas');
    const context = canvas.getContext('2d');
    if(!context) return 0;
    context.fillStyle = '#168249';
    context.fillRect(0, 0, canvas.width, canvas.height);
    const pixel = context.getImageData(64, 14, 1, 1).data;
    const passed = pixel[0] === 22 && pixel[1] === 130 && pixel[2] === 73 && pixel[3] === 255;
    canvas.hidden = !passed;
    return passed;
  }) != 0};
  return test_result(passed, passed ? "Canvas 2D · green pixel readback verified" : "Canvas 2D context or pixel readback failed");
}
