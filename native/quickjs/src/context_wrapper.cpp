#include "novelua/quickjs/context_wrapper.hpp"
#include <stdexcept>

namespace novelua::quickjs {

Context::Context(Runtime* runtime) {
    if (!runtime) {
        throw std::invalid_argument("Runtime cannot be null");
    }
    ctx_ = JS_NewContext(runtime->get());
    if (!ctx_) {
        throw std::runtime_error("Failed to create JSContext");
    }
}

Context::~Context() {
    if (ctx_) {
        JS_FreeContext(ctx_);
        ctx_ = nullptr;
    }
}

Value Context::eval(const std::string& script) {
    JSValue val = JS_Eval(ctx_, script.c_str(), script.length(), "<input>", JS_EVAL_TYPE_GLOBAL);
    return Value(ctx_, val);
}

Value Context::evalModule(const std::string& script) {
    JSValue val = JS_Eval(ctx_, script.c_str(), script.length(), "<input>", JS_EVAL_TYPE_MODULE);
    return Value(ctx_, val);
}

Value Context::getGlobal(const std::string& name) {
    JSValue global_obj = JS_GetGlobalObject(ctx_);
    JSValue val = JS_GetPropertyStr(ctx_, global_obj, name.c_str());
    JS_FreeValue(ctx_, global_obj);
    return Value(ctx_, val);
}

void Context::setGlobal(const std::string& name, JSValue value) {
    JSValue global_obj = JS_GetGlobalObject(ctx_);
    JS_SetPropertyStr(ctx_, global_obj, name.c_str(), value);
    JS_FreeValue(ctx_, global_obj);
}

} // namespace novelua::quickjs
