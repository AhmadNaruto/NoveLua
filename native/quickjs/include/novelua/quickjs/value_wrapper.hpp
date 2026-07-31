#pragma once

#include <quickjs.h>
#include <string>
#include <vector>
#include <memory>

namespace novelua::quickjs {

class Context;

class Value {
public:
    Value(JSContext* ctx, JSValue val);
    ~Value();

    Value(const Value& other);
    Value& operator=(const Value& other);
    
    Value(Value&& other) noexcept;
    Value& operator=(Value&& other) noexcept;

    JSValue get() const noexcept { return val_; }
    JSContext* getContext() const noexcept { return ctx_; }

    bool isNull() const noexcept;
    bool isUndefined() const noexcept;
    bool isNumber() const noexcept;
    bool isString() const noexcept;
    bool isBoolean() const noexcept;
    bool isObject() const noexcept;
    bool isArray() const noexcept;
    bool isFunction() const noexcept;

    std::string asString() const;
    int asInt() const;
    long asLong() const;
    double asDouble() const;
    bool asBoolean() const;

    std::vector<std::string> keys() const;
    Value getProperty(const std::string& name) const;
    void setProperty(const std::string& name, JSValue value);
    
    int arraySize() const;
    Value getArrayElement(int index) const;
    void setArrayElement(int index, JSValue value);
    void pushArrayElement(JSValue value);

    Value call(const std::vector<JSValue>& args) const;

private:
    JSContext* ctx_ = nullptr;
    JSValue val_;
};

} // namespace novelua::quickjs
