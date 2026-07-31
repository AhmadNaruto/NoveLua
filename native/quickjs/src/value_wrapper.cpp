#include "novelua/quickjs/value_wrapper.hpp"
#include <stdexcept>

namespace novelua::quickjs {

Value::Value(JSContext* ctx, JSValue val) : ctx_(ctx), val_(val) {
}

Value::~Value() {
    if (ctx_) {
        JS_FreeValue(ctx_, val_);
    }
}

Value::Value(const Value& other) : ctx_(other.ctx_), val_(JS_DupValue(other.ctx_, other.val_)) {
}

Value& Value::operator=(const Value& other) {
    if (this != &other) {
        if (ctx_) {
            JS_FreeValue(ctx_, val_);
        }
        ctx_ = other.ctx_;
        val_ = JS_DupValue(ctx_, other.val_);
    }
    return *this;
}

Value::Value(Value&& other) noexcept : ctx_(other.ctx_), val_(other.val_) {
    other.ctx_ = nullptr;
    other.val_ = JS_NULL;
}

Value& Value::operator=(Value&& other) noexcept {
    if (this != &other) {
        if (ctx_) {
            JS_FreeValue(ctx_, val_);
        }
        ctx_ = other.ctx_;
        val_ = other.val_;
        other.ctx_ = nullptr;
        other.val_ = JS_NULL;
    }
    return *this;
}

bool Value::isNull() const noexcept { return JS_IsNull(val_); }
bool Value::isUndefined() const noexcept { return JS_IsUndefined(val_); }
bool Value::isNumber() const noexcept { return JS_IsNumber(val_); }
bool Value::isString() const noexcept { return JS_IsString(val_); }
bool Value::isBoolean() const noexcept { return JS_IsBool(val_); }
bool Value::isObject() const noexcept { return JS_IsObject(val_); }
bool Value::isArray() const noexcept { return JS_IsArray(ctx_, val_) == 1; }
bool Value::isFunction() const noexcept { return JS_IsFunction(ctx_, val_); }

std::string Value::asString() const {
    const char* str = JS_ToCString(ctx_, val_);
    if (!str) {
        throw std::runtime_error("Cannot convert to string");
    }
    std::string res(str);
    JS_FreeCString(ctx_, str);
    return res;
}

int Value::asInt() const {
    int32_t res;
    if (JS_ToInt32(ctx_, &res, val_) < 0) throw std::runtime_error("Cannot convert to int");
    return res;
}

long Value::asLong() const {
    int64_t res;
    if (JS_ToInt64(ctx_, &res, val_) < 0) throw std::runtime_error("Cannot convert to long");
    return static_cast<long>(res);
}

double Value::asDouble() const {
    double res;
    if (JS_ToFloat64(ctx_, &res, val_) < 0) throw std::runtime_error("Cannot convert to double");
    return res;
}

bool Value::asBoolean() const {
    int res = JS_ToBool(ctx_, val_);
    if (res < 0) throw std::runtime_error("Cannot convert to boolean");
    return res != 0;
}

std::vector<std::string> Value::keys() const {
    std::vector<std::string> result;
    JSPropertyEnum* ptab;
    uint32_t plen;
    if (JS_GetOwnPropertyNames(ctx_, &ptab, &plen, val_, JS_GPN_STRING_MASK | JS_GPN_SYMBOL_MASK | JS_GPN_ENUM_ONLY) < 0) {
        return result;
    }
    for (uint32_t i = 0; i < plen; i++) {
        const char* prop = JS_AtomToCString(ctx_, ptab[i].atom);
        if (prop) {
            result.push_back(prop);
            JS_FreeCString(ctx_, prop);
        }
        JS_FreeAtom(ctx_, ptab[i].atom);
    }
    js_free(ctx_, ptab);
    return result;
}

Value Value::getProperty(const std::string& name) const {
    JSValue prop = JS_GetPropertyStr(ctx_, val_, name.c_str());
    return Value(ctx_, prop);
}

void Value::setProperty(const std::string& name, JSValue value) {
    JS_SetPropertyStr(ctx_, val_, name.c_str(), value);
}

int Value::arraySize() const {
    JSValue length_val = JS_GetPropertyStr(ctx_, val_, "length");
    int32_t length;
    if (JS_ToInt32(ctx_, &length, length_val) < 0) {
        JS_FreeValue(ctx_, length_val);
        return 0;
    }
    JS_FreeValue(ctx_, length_val);
    return length;
}

Value Value::getArrayElement(int index) const {
    JSValue elem = JS_GetPropertyUint32(ctx_, val_, index);
    return Value(ctx_, elem);
}

void Value::setArrayElement(int index, JSValue value) {
    JS_SetPropertyUint32(ctx_, val_, index, value);
}

void Value::pushArrayElement(JSValue value) {
    JSValue push_func = JS_GetPropertyStr(ctx_, val_, "push");
    if (JS_IsFunction(ctx_, push_func)) {
        JSValue ret = JS_Call(ctx_, push_func, val_, 1, &value);
        JS_FreeValue(ctx_, ret);
    }
    JS_FreeValue(ctx_, push_func);
    JS_FreeValue(ctx_, value);
}

Value Value::call(const std::vector<JSValue>& args) const {
    JSValue global_obj = JS_GetGlobalObject(ctx_);
    JSValue ret = JS_Call(ctx_, val_, global_obj, static_cast<int>(args.size()), const_cast<JSValue*>(args.data()));
    JS_FreeValue(ctx_, global_obj);
    return Value(ctx_, ret);
}

} // namespace novelua::quickjs
