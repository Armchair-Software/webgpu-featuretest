#include <string>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include "test_result.h"

namespace {
auto double_value(int value)->int {
  return value * 2;
}
}

EMSCRIPTEN_BINDINGS(feature_dom) {
  emscripten::function("double_value", &double_value);
}

auto main()->int {
  using emscripten::val;
  auto const document{val::global("document")};
  auto element{document.call<val>("createElement", std::string{"span"})};
  element.set("textContent", std::string{"Wasm DOM round trip"});
  document["body"].call<val>("appendChild", element);
  bool const dom_passed{element["isConnected"].as<bool>() &&
    element["textContent"].as<std::string>() == "Wasm DOM round trip"};
  element.call<void>("remove");
  bool const passed{dom_passed && val::module_property("double_value")(21).as<int>() == 42};
  return test_result(passed, passed ? "DOM update + JS → C++ callback verified" : "DOM or Embind callback mismatch");
}
