#include "test_result.h"

namespace {
struct unwind_check {
  unsigned int &count;
  ~unwind_check() { ++count; }
};

[[gnu::noinline]] auto throw_value(unsigned int &unwound)->void {
  unwind_check const guard{unwound};
  throw 42;
}
}

auto main()->int {
  unsigned int unwound{};
  bool caught{};
  try {
    try {
      throw_value(unwound);
    } catch(int value) {
      if(value != 42) return test_result(false, "Wrong exception payload");
      throw;
    }
  } catch(int value) {
    caught = value == 42;
  } catch(...) {
    return test_result(false, "Wrong exception type");
  }
  bool const passed{caught && unwound == 1};
  return test_result(passed, passed ? "JS exceptions · unwind verified" : "Exception payload or destructor count mismatch");
}
