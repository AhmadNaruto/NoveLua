#pragma once

#include <quickjs.h>
#include <memory>

namespace novelua::quickjs {

class Runtime {
public:
    Runtime();
    ~Runtime();

    Runtime(const Runtime&) = delete;
    Runtime& operator=(const Runtime&) = delete;

    JSRuntime* get() const noexcept { return runtime_; }

    void gc() noexcept;

private:
    JSRuntime* runtime_ = nullptr;
};

} // namespace novelua::quickjs
