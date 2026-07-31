#include "novelua/quickjs/runtime_wrapper.hpp"
#include <stdexcept>

namespace novelua::quickjs {

Runtime::Runtime() {
    runtime_ = JS_NewRuntime();
    if (!runtime_) {
        throw std::runtime_error("Failed to create JSRuntime");
    }
}

Runtime::~Runtime() {
    if (runtime_) {
        JS_FreeRuntime(runtime_);
        runtime_ = nullptr;
    }
}

void Runtime::gc() noexcept {
    if (runtime_) {
        JS_RunGC(runtime_);
    }
}

} // namespace novelua::quickjs
