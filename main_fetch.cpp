#include <cstring>
#include <emscripten/fetch.h>
#include "test_result.h"

auto main()->int {
  emscripten_fetch_attr_t attributes;
  emscripten_fetch_attr_init(&attributes);
  std::strcpy(attributes.requestMethod, "GET");
  attributes.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
  attributes.onsuccess = [](emscripten_fetch_t *request) {
    char constexpr expected[]{"Browser feature test fixture.\n"};
    bool const passed{request->status == 200 && request->numBytes == sizeof(expected) - 1 &&
      std::memcmp(request->data, expected, sizeof(expected) - 1) == 0};
    emscripten_fetch_close(request);
    test_result(passed, passed ? "C++ fetch · fixture bytes verified" : "Fetch status or payload mismatch");
  };
  attributes.onerror = [](emscripten_fetch_t *request) {
    emscripten_fetch_close(request);
    test_result(false, "C++ fetch failed to load fixture.txt");
  };
  if(emscripten_fetch(&attributes, "fixture.txt") == nullptr) {
    return test_result(false, "Could not start C++ fetch");
  }
  return 0; // Result arrives asynchronously; the Emscripten runtime stays alive.
}
