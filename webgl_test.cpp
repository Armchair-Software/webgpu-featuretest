#include "webgl_test.h"
#include <GLES2/gl2.h>
#include <emscripten/html5_webgl.h>
#include "test_result.h"

auto run_webgl_test(int version)->int {
  EmscriptenWebGLContextAttributes attributes;
  emscripten_webgl_init_context_attributes(&attributes);
  attributes.majorVersion = version;
  attributes.minorVersion = 0;
  attributes.alpha = false;
  attributes.depth = false;
  attributes.antialias = false;
  attributes.preserveDrawingBuffer = true;
  attributes.enableExtensionsByDefault = false;
  auto const context{emscripten_webgl_create_context("#canvas", &attributes)};
  if(context <= 0) return test_result(false, "WebGL context unavailable");
  if(emscripten_webgl_make_context_current(context) != EMSCRIPTEN_RESULT_SUCCESS) {
    emscripten_webgl_destroy_context(context);
    return test_result(false, "Could not make WebGL context current");
  }
  glClearColor(22.0f / 255.0f, 130.0f / 255.0f, 73.0f / 255.0f, 1.0f); // #168249
  glClear(GL_COLOR_BUFFER_BIT);
  unsigned char pixel[4]{};
  glReadPixels(64, 14, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
  bool const passed{glGetError() == GL_NO_ERROR && pixel[0] == 22 &&
    pixel[1] == 130 && pixel[2] == 73 && pixel[3] == 255};
  EM_ASM({ document.getElementById('canvas').hidden = !$0; }, passed);
  emscripten_webgl_destroy_context(context);
  return test_result(passed, passed ? "WebGL context · green pixel readback verified" : "WebGL error or pixel readback mismatch");
}
