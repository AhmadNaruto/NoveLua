#include <jni.h>
#include "vm_wrapper.hpp"
#include "table_wrapper.hpp"
#include "function_wrapper.hpp"
#include <novelua/common/jstring_utils.hpp>
#include <novelua/common/exceptions.hpp>
#include <map>
#include <mutex>
#include <string>
#include <vector>

using namespace novelua::luau;
using namespace novelua::common;

extern "C" {

static std::map<std::string, jobject> g_luau_callbacks;
static std::mutex g_luau_callbacks_mutex;
static JavaVM* g_jvm = nullptr;

// Helper: Push Java object onto Luau stack
static void PushJavaObject(JNIEnv* env, lua_State* L, jobject obj) {
    if (!obj) {
        lua_pushnil(L);
        return;
    }
    jclass objClass = env->GetObjectClass(obj);
    
    jclass stringClass = env->FindClass("java/lang/String");
    if (env->IsAssignableFrom(objClass, stringClass)) {
        jstring jstr = static_cast<jstring>(obj);
        const char* str = env->GetStringUTFChars(jstr, nullptr);
        lua_pushstring(L, str);
        env->ReleaseStringUTFChars(jstr, str);
        return;
    }

    jclass booleanClass = env->FindClass("java/lang/Boolean");
    if (env->IsAssignableFrom(objClass, booleanClass)) {
        jmethodID booleanValue = env->GetMethodID(booleanClass, "booleanValue", "()Z");
        jboolean val = env->CallBooleanMethod(obj, booleanValue);
        lua_pushboolean(L, val ? 1 : 0);
        return;
    }

    jclass numberClass = env->FindClass("java/lang/Number");
    if (env->IsAssignableFrom(objClass, numberClass)) {
        jmethodID doubleValue = env->GetMethodID(numberClass, "doubleValue", "()D");
        jdouble val = env->CallDoubleMethod(obj, doubleValue);
        lua_pushnumber(L, val);
        return;
    }

    jclass tableClass = env->FindClass("io/github/novelua/luau/Table");
    if (tableClass && env->IsInstanceOf(obj, tableClass)) {
        jfieldID handleField = env->GetFieldID(tableClass, "nativeHandle", "J");
        jlong handle = env->GetLongField(obj, handleField);
        auto* table = reinterpret_cast<Table*>(handle);
        if (table) {
            table->push();
        } else {
            lua_pushnil(L);
        }
        return;
    }

    jclass functionClass = env->FindClass("io/github/novelua/luau/Function");
    if (functionClass && env->IsInstanceOf(obj, functionClass)) {
        jfieldID handleField = env->GetFieldID(functionClass, "nativeHandle", "J");
        jlong handle = env->GetLongField(obj, handleField);
        auto* fn = reinterpret_cast<Function*>(handle);
        if (fn) {
            fn->push();
        } else {
            lua_pushnil(L);
        }
        return;
    }

    jclass mapClass = env->FindClass("java/util/Map");
    if (mapClass && env->IsAssignableFrom(objClass, mapClass)) {
        lua_newtable(L);
        jmethodID entrySetMethod = env->GetMethodID(mapClass, "entrySet", "()Ljava/util/Set;");
        jobject entrySet = env->CallObjectMethod(obj, entrySetMethod);
        jclass setClass = env->GetObjectClass(entrySet);
        jmethodID iteratorMethod = env->GetMethodID(setClass, "iterator", "()Ljava/util/Iterator;");
        jobject iterator = env->CallObjectMethod(entrySet, iteratorMethod);
        jclass iteratorClass = env->GetObjectClass(iterator);
        jmethodID hasNextMethod = env->GetMethodID(iteratorClass, "hasNext", "()Z");
        jmethodID nextMethod = env->GetMethodID(iteratorClass, "next", "()Ljava/lang/Object;");
        jclass entryClass = env->FindClass("java/util/Map$Entry");
        jmethodID getKeyMethod = env->GetMethodID(entryClass, "getKey", "()Ljava/lang/Object;");
        jmethodID getValueMethod = env->GetMethodID(entryClass, "getValue", "()Ljava/lang/Object;");
        while (env->CallBooleanMethod(iterator, hasNextMethod)) {
            jobject entry = env->CallObjectMethod(iterator, nextMethod);
            jobject key = env->CallObjectMethod(entry, getKeyMethod);
            jobject value = env->CallObjectMethod(entry, getValueMethod);
            PushJavaObject(env, L, key);
            PushJavaObject(env, L, value);
            lua_settable(L, -3);
            env->DeleteLocalRef(entry);
            env->DeleteLocalRef(key);
            env->DeleteLocalRef(value);
        }
        env->DeleteLocalRef(entrySet);
        env->DeleteLocalRef(iterator);
        return;
    }

    jmethodID toString = env->GetMethodID(objClass, "toString", "()Ljava/lang/String;");
    jstring jstr = static_cast<jstring>(env->CallObjectMethod(obj, toString));
    const char* str = env->GetStringUTFChars(jstr, nullptr);
    lua_pushstring(L, str);
    env->ReleaseStringUTFChars(jstr, str);
}

// Helper: Convert Luau stack value to Java Object
static jobject GetLuaValueAsJavaObject(JNIEnv* env, lua_State* L, int index) {
    int type = lua_type(L, index);
    if (type == LUA_TNIL) {
        return nullptr;
    }
    if (type == LUA_TBOOLEAN) {
        jclass booleanClass = env->FindClass("java/lang/Boolean");
        jmethodID valueOf = env->GetStaticMethodID(booleanClass, "valueOf", "(Z)Ljava/lang/Boolean;");
        return env->CallStaticObjectMethod(booleanClass, valueOf, lua_toboolean(L, index) ? JNI_TRUE : JNI_FALSE);
    }
    if (type == LUA_TNUMBER) {
        jclass doubleClass = env->FindClass("java/lang/Double");
        jmethodID valueOf = env->GetStaticMethodID(doubleClass, "valueOf", "(D)Ljava/lang/Double;");
        return env->CallStaticObjectMethod(doubleClass, valueOf, lua_tonumber(L, index));
    }
    if (type == LUA_TSTRING) {
        return env->NewStringUTF(lua_tostring(L, index));
    }
    if (type == LUA_TTABLE) {
        lua_pushvalue(L, index);
        int ref = luaL_ref(L, LUA_REGISTRYINDEX);
        auto* table = new Table(L, ref);
        jclass tableClass = env->FindClass("io/github/novelua/luau/Table");
        jmethodID init = env->GetMethodID(tableClass, "<init>", "(J)V");
        return env->NewObject(tableClass, init, reinterpret_cast<jlong>(table));
    }
    if (type == LUA_TFUNCTION) {
        lua_pushvalue(L, index);
        int ref = luaL_ref(L, LUA_REGISTRYINDEX);
        auto* fn = new Function(L, ref);
        jclass functionClass = env->FindClass("io/github/novelua/luau/Function");
        jmethodID init = env->GetMethodID(functionClass, "<init>", "(J)V");
        return env->NewObject(functionClass, init, reinterpret_cast<jlong>(fn));
    }
    return env->NewStringUTF(luaL_typename(L, index));
}

// ── VM Callbacks ─────────────────────────────────────────────────────────────

static int Luau_callback_handler(lua_State* L) {
    const char* name = lua_tostring(L, lua_upvalueindex(1));
    if (!name) return 0;

    std::string key = std::to_string(reinterpret_cast<uintptr_t>(L)) + "_" + name;

    jobject callback_obj = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_luau_callbacks_mutex);
        auto it = g_luau_callbacks.find(key);
        if (it != g_luau_callbacks.end()) {
            callback_obj = it->second;
        }
    }

    if (!callback_obj) return 0;

    JNIEnv* env = nullptr;
    bool needs_detach = false;
    jint get_env_res = g_jvm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
    if (get_env_res == JNI_EDETACHED) {
        g_jvm->AttachCurrentThread(reinterpret_cast<void**>(&env), nullptr);
        needs_detach = true;
    }

    if (!env) return 0;

    int narg = lua_gettop(L);
    jclass stringClass = env->FindClass("java/lang/String");
    jobjectArray argsArr = env->NewObjectArray(narg, stringClass, nullptr);
    for (int i = 1; i <= narg; ++i) {
        std::string argStr;
        if (lua_isstring(L, i)) {
            argStr = lua_tostring(L, i);
        } else if (lua_isboolean(L, i)) {
            argStr = lua_toboolean(L, i) ? "true" : "false";
        } else if (lua_isnumber(L, i)) {
            argStr = std::to_string(lua_tonumber(L, i));
        } else if (lua_isnil(L, i)) {
            argStr = "nil";
        } else {
            argStr = "[object]";
        }
        jstring js = env->NewStringUTF(argStr.c_str());
        env->SetObjectArrayElement(argsArr, i - 1, js);
        env->DeleteLocalRef(js);
    }

    jclass callbackClass = env->GetObjectClass(callback_obj);
    jmethodID callMethod = env->GetMethodID(callbackClass, "call", "([Ljava/lang/String;)Ljava/lang/String;");
    if (!callMethod) {
        if (needs_detach) g_jvm->DetachCurrentThread();
        return 0;
    }

    jstring resStr = static_cast<jstring>(env->CallObjectMethod(callback_obj, callMethod, argsArr));
    
    if (resStr) {
        const char* resChars = env->GetStringUTFChars(resStr, nullptr);
        lua_pushstring(L, resChars);
        env->ReleaseStringUTFChars(resStr, resChars);
    } else {
        lua_pushnil(L);
    }

    if (needs_detach) g_jvm->DetachCurrentThread();
    return 1;
}

// ── VM JNI Methods ────────────────────────────────────────────────────────────

static jlong VM_create(JNIEnv* env, jobject) {
    try {
        return reinterpret_cast<jlong>(new VM());
    } catch (const std::exception& e) {
        ThrowRuntimeException(env, e.what());
        return 0;
    }
}

static void VM_destroy(JNIEnv* env, jobject, jlong handle) {
    auto* vm = reinterpret_cast<VM*>(handle);
    if (vm) {
        lua_State* L = vm->getState();
        std::string prefix = std::to_string(reinterpret_cast<uintptr_t>(L)) + "_";
        {
            std::lock_guard<std::mutex> lock(g_luau_callbacks_mutex);
            for (auto it = g_luau_callbacks.begin(); it != g_luau_callbacks.end(); ) {
                if (it->first.rfind(prefix, 0) == 0) {
                    env->DeleteGlobalRef(it->second);
                    it = g_luau_callbacks.erase(it);
                } else {
                    ++it;
                }
            }
        }
        delete vm;
    }
}

static jstring VM_eval(JNIEnv* env, jobject, jlong handle, jstring jscript) {
    auto* vm = reinterpret_cast<VM*>(handle);
    if (!vm) return nullptr;
    try {
        std::string script = ToStdString(env, jscript);
        std::string res = vm->eval(script);
        return env->NewStringUTF(res.c_str());
    } catch (const std::exception& e) {
        ThrowRuntimeException(env, e.what());
        return nullptr;
    }
}

static void VM_load(JNIEnv* env, jobject, jlong handle, jbyteArray jbytes) {
    auto* vm = reinterpret_cast<VM*>(handle);
    if (!vm) return;
    jsize len = env->GetArrayLength(jbytes);
    std::vector<char> bytecode(len);
    env->GetByteArrayRegion(jbytes, 0, len, reinterpret_cast<jbyte*>(bytecode.data()));
    try {
        vm->load(std::string(bytecode.begin(), bytecode.end()));
    } catch (const std::exception& e) {
        ThrowRuntimeException(env, e.what());
    }
}

static void VM_execute(JNIEnv* env, jobject, jlong handle) {
    auto* vm = reinterpret_cast<VM*>(handle);
    if (!vm) return;
    try {
        vm->execute();
    } catch (const std::exception& e) {
        ThrowRuntimeException(env, e.what());
    }
}

static jlong VM_createTable(JNIEnv*, jobject, jlong handle) {
    auto* vm = reinterpret_cast<VM*>(handle);
    if (!vm) return 0;
    lua_State* L = vm->getState();
    lua_newtable(L);
    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    return reinterpret_cast<jlong>(new Table(L, ref));
}

static jobject VM_getGlobal(JNIEnv* env, jobject, jlong handle, jstring jname) {
    auto* vm = reinterpret_cast<VM*>(handle);
    if (!vm) return nullptr;
    std::string name = ToStdString(env, jname);
    lua_State* L = vm->getState();
    lua_getglobal(L, name.c_str());
    jobject res = GetLuaValueAsJavaObject(env, L, -1);
    lua_pop(L, 1);
    return res;
}

static void VM_setGlobal(JNIEnv* env, jobject, jlong handle, jstring jname, jobject jval) {
    auto* vm = reinterpret_cast<VM*>(handle);
    if (!vm) return;
    std::string name = ToStdString(env, jname);
    lua_State* L = vm->getState();
    PushJavaObject(env, L, jval);
    lua_setglobal(L, name.c_str());
}

static jlong VM_globals(JNIEnv*, jobject, jlong handle) {
    auto* vm = reinterpret_cast<VM*>(handle);
    if (!vm) return 0;
    lua_State* L = vm->getState();
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    return reinterpret_cast<jlong>(new Table(L, ref));
}

static jlong VM_registry(JNIEnv*, jobject, jlong handle) {
    auto* vm = reinterpret_cast<VM*>(handle);
    if (!vm) return 0;
    lua_State* L = vm->getState();
    lua_pushvalue(L, LUA_REGISTRYINDEX);
    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    return reinterpret_cast<jlong>(new Table(L, ref));
}

static void VM_registerCallback(JNIEnv* env, jobject, jlong handle, jstring jname, jobject callback) {
    auto* vm = reinterpret_cast<VM*>(handle);
    if (!vm) return;
    std::string name = ToStdString(env, jname);
    lua_State* L = vm->getState();

    jobject global_callback = env->NewGlobalRef(callback);

    std::string key = std::to_string(reinterpret_cast<uintptr_t>(L)) + "_" + name;
    {
        std::lock_guard<std::mutex> lock(g_luau_callbacks_mutex);
        auto it = g_luau_callbacks.find(key);
        if (it != g_luau_callbacks.end()) {
            env->DeleteGlobalRef(it->second);
        }
        g_luau_callbacks[key] = global_callback;
    }

    lua_pushstring(L, name.c_str());
    lua_pushcclosure(L, Luau_callback_handler, 1);
    lua_setglobal(L, name.c_str());
}

// ── Table JNI Methods ─────────────────────────────────────────────────────────

static jobject Table_get(JNIEnv* env, jobject, jlong handle, jobject key) {
    auto* table = reinterpret_cast<Table*>(handle);
    if (!table || !table->getState()) return nullptr;
    lua_State* L = table->getState();
    table->push();
    PushJavaObject(env, L, key);
    lua_gettable(L, -2);
    jobject res = GetLuaValueAsJavaObject(env, L, -1);
    lua_pop(L, 2);
    return res;
}

static void Table_set(JNIEnv* env, jobject, jlong handle, jobject key, jobject value) {
    auto* table = reinterpret_cast<Table*>(handle);
    if (!table || !table->getState()) return;
    lua_State* L = table->getState();
    table->push();
    PushJavaObject(env, L, key);
    PushJavaObject(env, L, value);
    lua_settable(L, -3);
    lua_pop(L, 1);
}

static void Table_remove(JNIEnv* env, jobject, jlong handle, jobject key) {
    auto* table = reinterpret_cast<Table*>(handle);
    if (!table || !table->getState()) return;
    lua_State* L = table->getState();
    table->push();
    PushJavaObject(env, L, key);
    lua_pushnil(L);
    lua_settable(L, -3);
    lua_pop(L, 1);
}

static void Table_clear(JNIEnv*, jobject, jlong handle) {
    auto* table = reinterpret_cast<Table*>(handle);
    if (!table || !table->getState()) return;
    lua_State* L = table->getState();
    table->push();
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_pushnil(L);
        lua_settable(L, -5);
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
}

static jboolean Table_contains(JNIEnv* env, jobject, jlong handle, jobject key) {
    auto* table = reinterpret_cast<Table*>(handle);
    if (!table || !table->getState()) return JNI_FALSE;
    lua_State* L = table->getState();
    table->push();
    PushJavaObject(env, L, key);
    lua_gettable(L, -2);
    bool res = !lua_isnil(L, -1);
    lua_pop(L, 2);
    return res ? JNI_TRUE : JNI_FALSE;
}

static jint Table_size(JNIEnv*, jobject, jlong handle) {
    auto* table = reinterpret_cast<Table*>(handle);
    if (!table || !table->getState()) return 0;
    lua_State* L = table->getState();
    table->push();
    int size = 0;
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        size++;
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    return size;
}

static jobject Table_keys(JNIEnv* env, jobject, jlong handle) {
    auto* table = reinterpret_cast<Table*>(handle);
    if (!table || !table->getState()) return nullptr;
    lua_State* L = table->getState();
    table->push();
    jclass listClass = env->FindClass("java/util/ArrayList");
    jmethodID init = env->GetMethodID(listClass, "<init>", "()V");
    jobject list = env->NewObject(listClass, init);
    jmethodID add = env->GetMethodID(listClass, "add", "(Ljava/lang/Object;)Z");

    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        jobject key = GetLuaValueAsJavaObject(env, L, -2);
        if (key) {
            env->CallBooleanMethod(list, add, key);
            env->DeleteLocalRef(key);
        }
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    return list;
}

static jobject Table_values(JNIEnv* env, jobject, jlong handle) {
    auto* table = reinterpret_cast<Table*>(handle);
    if (!table || !table->getState()) return nullptr;
    lua_State* L = table->getState();
    table->push();
    jclass listClass = env->FindClass("java/util/ArrayList");
    jmethodID init = env->GetMethodID(listClass, "<init>", "()V");
    jobject list = env->NewObject(listClass, init);
    jmethodID add = env->GetMethodID(listClass, "add", "(Ljava/lang/Object;)Z");

    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        jobject val = GetLuaValueAsJavaObject(env, L, -1);
        if (val) {
            env->CallBooleanMethod(list, add, val);
            env->DeleteLocalRef(val);
        }
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    return list;
}

static void Table_destroy(JNIEnv*, jobject, jlong handle) {
    if (handle != 0) {
        delete reinterpret_cast<Table*>(handle);
    }
}

// ── Function JNI Methods ──────────────────────────────────────────────────────

static jobject Function_call(JNIEnv* env, jobject, jlong handle, jobjectArray args) {
    auto* fn = reinterpret_cast<Function*>(handle);
    if (!fn || !fn->getState()) return nullptr;
    lua_State* L = fn->getState();
    
    fn->push();

    jsize len = 0;
    if (args) {
        len = env->GetArrayLength(args);
        for (jsize i = 0; i < len; ++i) {
            jobject arg = env->GetObjectArrayElement(args, i);
            PushJavaObject(env, L, arg);
            env->DeleteLocalRef(arg);
        }
    }

    if (lua_pcall(L, len, 1, 0) != 0) {
        std::string err = lua_tostring(L, -1);
        lua_pop(L, 1);
        ThrowRuntimeException(env, err.c_str());
        return nullptr;
    }

    jobject res = GetLuaValueAsJavaObject(env, L, -1);
    lua_pop(L, 1);
    return res;
}

static void Function_destroy(JNIEnv*, jobject, jlong handle) {
    if (handle != 0) {
        delete reinterpret_cast<Function*>(handle);
    }
}

// ── Compiler JNI Methods ──────────────────────────────────────────────────────

static jbyteArray Compiler_compile(JNIEnv* env, jobject, jstring jsource) {
    std::string source = ToStdString(env, jsource);
    size_t outSize = 0;
    char* bytecode = luau_compile(source.c_str(), source.length(), nullptr, &outSize);
    if (!bytecode) return nullptr;

    jbyteArray arr = env->NewByteArray(outSize);
    env->SetByteArrayRegion(arr, 0, outSize, reinterpret_cast<const jbyte*>(bytecode));
    free(bytecode);
    return arr;
}

// ── Method Registers ──────────────────────────────────────────────────────────

static const JNINativeMethod gVMMethods[] = {
    {"nativeCreate", "()J", reinterpret_cast<void*>(VM_create)},
    {"nativeDestroy", "(J)V", reinterpret_cast<void*>(VM_destroy)},
    {"nativeEval", "(JLjava/lang/String;)Ljava/lang/String;", reinterpret_cast<void*>(VM_eval)},
    {"nativeLoad", "(J[B)V", reinterpret_cast<void*>(VM_load)},
    {"nativeExecute", "(J)V", reinterpret_cast<void*>(VM_execute)},
    {"nativeCreateTable", "(J)J", reinterpret_cast<void*>(VM_createTable)},
    {"nativeGetGlobal", "(JLjava/lang/String;)Ljava/lang/Object;", reinterpret_cast<void*>(VM_getGlobal)},
    {"nativeSetGlobal", "(JLjava/lang/String;Ljava/lang/Object;)V", reinterpret_cast<void*>(VM_setGlobal)},
    {"nativeGetGlobals", "(J)J", reinterpret_cast<void*>(VM_globals)},
    {"nativeGetRegistry", "(J)J", reinterpret_cast<void*>(VM_registry)},
    {"nativeRegisterCallback", "(JLjava/lang/String;Lio/github/novelua/luau/VMCallback;)V", reinterpret_cast<void*>(VM_registerCallback)}
};

static const JNINativeMethod gTableMethods[] = {
    {"nativeSize", "(J)I", reinterpret_cast<void*>(Table_size)},
    {"nativeGet", "(JLjava/lang/Object;)Ljava/lang/Object;", reinterpret_cast<void*>(Table_get)},
    {"nativeSet", "(JLjava/lang/Object;Ljava/lang/Object;)V", reinterpret_cast<void*>(Table_set)},
    {"nativeRemove", "(JLjava/lang/Object;)V", reinterpret_cast<void*>(Table_remove)},
    {"nativeClear", "(J)V", reinterpret_cast<void*>(Table_clear)},
    {"nativeContains", "(JLjava/lang/Object;)Z", reinterpret_cast<void*>(Table_contains)},
    {"nativeKeys", "(J)Ljava/util/List;", reinterpret_cast<void*>(Table_keys)},
    {"nativeValues", "(J)Ljava/util/List;", reinterpret_cast<void*>(Table_values)},
    {"nativeDestroy", "(J)V", reinterpret_cast<void*>(Table_destroy)}
};

static const JNINativeMethod gFunctionMethods[] = {
    {"nativeCall", "(J[Ljava/lang/Object;)Ljava/lang/Object;", reinterpret_cast<void*>(Function_call)},
    {"nativeDestroy", "(J)V", reinterpret_cast<void*>(Function_destroy)}
};

static const JNINativeMethod gCompilerMethods[] = {
    {"nativeCompile", "(Ljava/lang/String;)[B", reinterpret_cast<void*>(Compiler_compile)}
};

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
    g_jvm = vm;
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }

    jclass vmClass = env->FindClass("io/github/novelua/luau/VM");
    if (vmClass && env->RegisterNatives(vmClass, gVMMethods, sizeof(gVMMethods) / sizeof(gVMMethods[0])) >= 0) {
        // Continue
    } else {
        return JNI_ERR;
    }

    jclass tableClass = env->FindClass("io/github/novelua/luau/Table");
    if (tableClass && env->RegisterNatives(tableClass, gTableMethods, sizeof(gTableMethods) / sizeof(gTableMethods[0])) >= 0) {
        // Continue
    } else {
        return JNI_ERR;
    }

    jclass functionClass = env->FindClass("io/github/novelua/luau/Function");
    if (functionClass && env->RegisterNatives(functionClass, gFunctionMethods, sizeof(gFunctionMethods) / sizeof(gFunctionMethods[0])) >= 0) {
        // Continue
    } else {
        return JNI_ERR;
    }

    jclass compilerClass = env->FindClass("io/github/novelua/luau/Compiler");
    if (compilerClass && env->RegisterNatives(compilerClass, gCompilerMethods, sizeof(gCompilerMethods) / sizeof(gCompilerMethods[0])) >= 0) {
        // Continue
    } else {
        return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}

} // extern "C"


