#pragma once

#include <quickjs.h>
#include "novelua/quickjs/runtime_wrapper.hpp"
#include "novelua/quickjs/value_wrapper.hpp"
#include <string>

namespace novelua::quickjs {

class Context {
public:
    explicit Context(Runtime* runtime);
    ~Context();

    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;

    JSContext* get() const noexcept { return ctx_; }

    Value eval(const std::string& script);
    Value evalModule(const std::string& script);

    Value getGlobal(const std::string& name);
    void setGlobal(const std::string& name, JSValue value);

private:
    JSContext* ctx_ = nullptr;
};

} // namespace novelua::quickjs
