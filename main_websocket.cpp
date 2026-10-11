#include <emscripten/websocket.h>
#include "test_result.h"

auto main()->int {
  // This deliberately checks only the C API bridge. Creating a socket would
  // initiate a network connection and introduce an external server dependency.
  bool const passed{emscripten_websocket_is_supported()};
  return test_result(passed, passed ? "C API available · connectivity untested" : "Emscripten WebSocket API unavailable");
}
