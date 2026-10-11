#include "test_result.h"

namespace {
EM_JS(bool, check_storage, (), {
  const key = 'webgpu-featuretest.' + Date.now() + '.' + Math.random();
  try {
    const storage = window.localStorage;
    try {
      storage.setItem(key, 'Wasm storage round trip');
      if(storage.getItem(key) !== 'Wasm storage round trip') return false;
      storage.removeItem(key);
      return storage.getItem(key) === null;
    } finally {
      storage.removeItem(key);
    }
  } catch(error) {
    window.report_result(false, 'localStorage: ' + error.message);
    return false;
  }
});
}

auto main()->int {
  bool const passed{check_storage()};
  return test_result(passed, passed ? "localStorage write / read / remove verified" : "Storage round trip failed");
}
