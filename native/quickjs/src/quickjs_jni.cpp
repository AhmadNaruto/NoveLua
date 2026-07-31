#include <jni.h>
#include "novelua/quickjs/runtime_wrapper.hpp"
#include "novelua/quickjs/context_wrapper.hpp"
#include "novelua/quickjs/value_wrapper.hpp"
#include "novelua/common/exceptions.hpp"
#include "novelua/common/jstring_utils.hpp"
#include <memory>

using namespace novelua::quickjs;
using namespace novelua::common;

static jlong Runtime_create(JNIEnv* env, jobject) {
    try { return reinterpret_cast<jlong>(new Runtime()); }
    catch (...) { ThrowRuntimeException(env, "Failed to create Runtime"); return 0; }
}
static void Runtime_destroy(JNIEnv*, jobject, jlong handle) { delete reinterpret_cast<Runtime*>(handle); }
static void Runtime_gc(JNIEnv*, jobject, jlong handle) { if (handle) reinterpret_cast<Runtime*>(handle)->gc(); }

static jlong Context_create(JNIEnv* env, jobject, jlong rtHandle) {
    try { return reinterpret_cast<jlong>(new Context(reinterpret_cast<Runtime*>(rtHandle))); }
    catch (...) { ThrowRuntimeException(env, "Failed to create Context"); return 0; }
}
static void Context_destroy(JNIEnv*, jobject, jlong handle) { delete reinterpret_cast<Context*>(handle); }

static jlong Context_eval(JNIEnv* env, jobject, jlong handle, jstring script, jboolean isModule) {
    try {
        auto val = isModule ? reinterpret_cast<Context*>(handle)->evalModule(ToStdString(env, script))
                            : reinterpret_cast<Context*>(handle)->eval(ToStdString(env, script));
        return reinterpret_cast<jlong>(new Value(std::move(val)));
    } catch (const std::exception& e) { ThrowRuntimeException(env, e.what()); return 0; }
}
static jlong Context_getGlobal(JNIEnv* env, jobject, jlong handle, jstring name) {
    try {
        auto val = reinterpret_cast<Context*>(handle)->getGlobal(ToStdString(env, name));
        return reinterpret_cast<jlong>(new Value(std::move(val)));
    } catch (...) { ThrowRuntimeException(env, "Failed to get global"); return 0; }
}
static void Context_setGlobal(JNIEnv* env, jobject, jlong handle, jstring name, jlong valueHandle) {
    try {
        auto* ctx = reinterpret_cast<Context*>(handle);
        auto* val = reinterpret_cast<Value*>(valueHandle);
        ctx->setGlobal(ToStdString(env, name), JS_DupValue(ctx->get(), val->get()));
    } catch (...) { ThrowRuntimeException(env, "Failed to set global"); }
}

static jlong Context_createInt(JNIEnv*, jobject, jlong handle, jint value) {
    auto* ctx = reinterpret_cast<Context*>(handle);
    return reinterpret_cast<jlong>(new Value(ctx->get(), JS_NewInt32(ctx->get(), value)));
}
static jlong Context_createDouble(JNIEnv*, jobject, jlong handle, jdouble value) {
    auto* ctx = reinterpret_cast<Context*>(handle);
    return reinterpret_cast<jlong>(new Value(ctx->get(), JS_NewFloat64(ctx->get(), value)));
}
static jlong Context_createBoolean(JNIEnv*, jobject, jlong handle, jboolean value) {
    auto* ctx = reinterpret_cast<Context*>(handle);
    return reinterpret_cast<jlong>(new Value(ctx->get(), JS_NewBool(ctx->get(), value)));
}
static jlong Context_createString(JNIEnv* env, jobject, jlong handle, jstring value) {
    auto* ctx = reinterpret_cast<Context*>(handle);
    std::string s = ToStdString(env, value);
    return reinterpret_cast<jlong>(new Value(ctx->get(), JS_NewStringLen(ctx->get(), s.c_str(), s.length())));
}
static jlong Context_createNull(JNIEnv*, jobject, jlong handle) {
    auto* ctx = reinterpret_cast<Context*>(handle);
    return reinterpret_cast<jlong>(new Value(ctx->get(), JS_NULL));
}

static void Value_destroy(JNIEnv*, jobject, jlong handle) { delete reinterpret_cast<Value*>(handle); }
static jboolean Value_isNull(JNIEnv*, jobject, jlong handle) { return reinterpret_cast<Value*>(handle)->isNull(); }
static jboolean Value_isUndefined(JNIEnv*, jobject, jlong handle) { return reinterpret_cast<Value*>(handle)->isUndefined(); }
static jboolean Value_isNumber(JNIEnv*, jobject, jlong handle) { return reinterpret_cast<Value*>(handle)->isNumber(); }
static jboolean Value_isString(JNIEnv*, jobject, jlong handle) { return reinterpret_cast<Value*>(handle)->isString(); }
static jboolean Value_isBoolean(JNIEnv*, jobject, jlong handle) { return reinterpret_cast<Value*>(handle)->isBoolean(); }
static jboolean Value_isObject(JNIEnv*, jobject, jlong handle) { return reinterpret_cast<Value*>(handle)->isObject(); }
static jboolean Value_isArray(JNIEnv*, jobject, jlong handle) { return reinterpret_cast<Value*>(handle)->isArray(); }
static jboolean Value_isFunction(JNIEnv*, jobject, jlong handle) { return reinterpret_cast<Value*>(handle)->isFunction(); }

static jstring Value_asString(JNIEnv* env, jobject, jlong handle) {
    try { return ToJString(env, reinterpret_cast<Value*>(handle)->asString()); }
    catch (...) { ThrowRuntimeException(env, "Failed to convert to string"); return nullptr; }
}
static jint Value_asInt(JNIEnv* env, jobject, jlong handle) {
    try { return reinterpret_cast<Value*>(handle)->asInt(); }
    catch (...) { ThrowRuntimeException(env, "Failed to convert to int"); return 0; }
}
static jlong Value_asLong(JNIEnv* env, jobject, jlong handle) {
    try { return reinterpret_cast<Value*>(handle)->asLong(); }
    catch (...) { ThrowRuntimeException(env, "Failed to convert to long"); return 0; }
}
static jdouble Value_asDouble(JNIEnv* env, jobject, jlong handle) {
    try { return reinterpret_cast<Value*>(handle)->asDouble(); }
    catch (...) { ThrowRuntimeException(env, "Failed to convert to double"); return 0; }
}
static jboolean Value_asBoolean(JNIEnv* env, jobject, jlong handle) {
    try { return reinterpret_cast<Value*>(handle)->asBoolean(); }
    catch (...) { ThrowRuntimeException(env, "Failed to convert to boolean"); return false; }
}

static jobjectArray Object_keys(JNIEnv* env, jobject, jlong handle) {
    try {
        auto keys = reinterpret_cast<Value*>(handle)->keys();
        jclass strCls = env->FindClass("java/lang/String");
        jobjectArray arr = env->NewObjectArray(keys.size(), strCls, nullptr);
        for (size_t i = 0; i < keys.size(); ++i) env->SetObjectArrayElement(arr, i, ToJString(env, keys[i]));
        return arr;
    } catch (...) { ThrowRuntimeException(env, "Failed to get keys"); return nullptr; }
}
static jlong Object_get(JNIEnv* env, jobject, jlong handle, jstring name) {
    try { return reinterpret_cast<jlong>(new Value(reinterpret_cast<Value*>(handle)->getProperty(ToStdString(env, name)))); }
    catch (...) { ThrowRuntimeException(env, "Failed to get property"); return 0; }
}
static void Object_set(JNIEnv* env, jobject, jlong handle, jstring name, jlong valHandle) {
    try {
        auto* val = reinterpret_cast<Value*>(handle);
        auto* v = reinterpret_cast<Value*>(valHandle);
        val->setProperty(ToStdString(env, name), JS_DupValue(v->getContext(), v->get()));
    } catch (...) { ThrowRuntimeException(env, "Failed to set property"); }
}

static jint Array_getSize(JNIEnv*, jobject, jlong handle) { return reinterpret_cast<Value*>(handle)->arraySize(); }
static jlong Array_get(JNIEnv* env, jobject, jlong handle, jint idx) {
    try { return reinterpret_cast<jlong>(new Value(reinterpret_cast<Value*>(handle)->getArrayElement(idx))); }
    catch (...) { ThrowRuntimeException(env, "Failed to get array element"); return 0; }
}
static void Array_set(JNIEnv* env, jobject, jlong handle, jint idx, jlong valHandle) {
    try {
        auto* val = reinterpret_cast<Value*>(handle);
        auto* v = reinterpret_cast<Value*>(valHandle);
        val->setArrayElement(idx, JS_DupValue(v->getContext(), v->get()));
    } catch (...) { ThrowRuntimeException(env, "Failed to set array element"); }
}
static void Array_push(JNIEnv* env, jobject, jlong handle, jlong valHandle) {
    try {
        auto* val = reinterpret_cast<Value*>(handle);
        auto* v = reinterpret_cast<Value*>(valHandle);
        val->pushArrayElement(JS_DupValue(v->getContext(), v->get()));
    } catch (...) { ThrowRuntimeException(env, "Failed to push array element"); }
}

static jlong Function_call(JNIEnv* env, jobject, jlong handle, jlongArray argsHandle) {
    try {
        auto* val = reinterpret_cast<Value*>(handle);
        jsize len = env->GetArrayLength(argsHandle);
        jlong* elems = env->GetLongArrayElements(argsHandle, nullptr);
        std::vector<JSValue> args;
        for (jsize i = 0; i < len; ++i) {
            auto* v = reinterpret_cast<Value*>(elems[i]);
            args.push_back(v->get()); // Not duping because JS_Call doesn't consume arguments
        }
        Value ret = val->call(args);
        env->ReleaseLongArrayElements(argsHandle, elems, JNI_ABORT);
        return reinterpret_cast<jlong>(new Value(std::move(ret)));
    } catch (...) { ThrowRuntimeException(env, "Failed to call function"); return 0; }
}

static JNINativeMethod methods[] = {
    {"createRuntime", "()J", (void*)Runtime_create},
    {"destroyRuntime", "(J)V", (void*)Runtime_destroy},
    {"gc", "(J)V", (void*)Runtime_gc},
    {"createContext", "(J)J", (void*)Context_create},
    {"destroyContext", "(J)V", (void*)Context_destroy},
    {"evalScript", "(JLjava/lang/String;Z)J", (void*)Context_eval},
    {"getGlobal", "(JLjava/lang/String;)J", (void*)Context_getGlobal},
    {"setGlobal", "(JLjava/lang/String;J)V", (void*)Context_setGlobal},
    {"createInt", "(JI)J", (void*)Context_createInt},
    {"createDouble", "(JD)J", (void*)Context_createDouble},
    {"createBoolean", "(JZ)J", (void*)Context_createBoolean},
    {"createString", "(JLjava/lang/String;)J", (void*)Context_createString},
    {"createNull", "(J)J", (void*)Context_createNull},
    {"destroyValue", "(J)V", (void*)Value_destroy},
    {"isNull", "(J)Z", (void*)Value_isNull},
    {"isUndefined", "(J)Z", (void*)Value_isUndefined},
    {"isNumber", "(J)Z", (void*)Value_isNumber},
    {"isString", "(J)Z", (void*)Value_isString},
    {"isBoolean", "(J)Z", (void*)Value_isBoolean},
    {"isObject", "(J)Z", (void*)Value_isObject},
    {"isArray", "(J)Z", (void*)Value_isArray},
    {"isFunction", "(J)Z", (void*)Value_isFunction},
    {"asString", "(J)Ljava/lang/String;", (void*)Value_asString},
    {"asInt", "(J)I", (void*)Value_asInt},
    {"asLong", "(J)J", (void*)Value_asLong},
    {"asDouble", "(J)D", (void*)Value_asDouble},
    {"asBoolean", "(J)Z", (void*)Value_asBoolean},
    {"objectKeys", "(J)[Ljava/lang/String;", (void*)Object_keys},
    {"objectGet", "(JLjava/lang/String;)J", (void*)Object_get},
    {"objectSet", "(JLjava/lang/String;J)V", (void*)Object_set},
    {"arrayGetSize", "(J)I", (void*)Array_getSize},
    {"arrayGet", "(JI)J", (void*)Array_get},
    {"arraySet", "(JIJ)V", (void*)Array_set},
    {"arrayPush", "(JJ)V", (void*)Array_push},
    {"functionCall", "(J[J)J", (void*)Function_call},
};

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) return JNI_ERR;
    jclass clazz = env->FindClass("io/github/novelua/js/QuickJSNative");
    if (!clazz) return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) < 0) return JNI_ERR;
    return JNI_VERSION_1_6;
}
