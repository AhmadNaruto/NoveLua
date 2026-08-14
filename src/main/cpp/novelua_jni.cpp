#include <jni.h>
#include <cstdlib>
#include <cstring>
#include "lua.h"
#include "lualib.h"
#include "luaujava.h"
#include "luaujavac.h"

static JavaVM* g_jvm = nullptr;
static jclass g_luaStateCls = nullptr;
static jmethodID g_luaStateCtor = nullptr;
static jclass g_luaFuncCls = nullptr;
static jmethodID g_luaFuncInvoke = nullptr;
static jclass g_luaYieldableFuncCls = nullptr;
static jmethodID g_luaYieldableFuncResume = nullptr;

// Thread-local pending exception
static thread_local jobject g_pendingException = nullptr;

static void release_global_ref(jobject ref) {
    if (ref && g_jvm) {
        JNIEnv* env = nullptr;
        bool attached = false;
        if (g_jvm->GetEnv((void**)&env, JNI_VERSION_1_6) == JNI_EDETACHED) {
#ifdef __ANDROID__
            if (g_jvm->AttachCurrentThread(&env, nullptr) == JNI_OK) {
#else
            if (g_jvm->AttachCurrentThread((void**)&env, nullptr) == JNI_OK) {
#endif
                attached = true;
            }
        }
        if (env) {
            env->DeleteGlobalRef(ref);
        }
        if (attached) {
            g_jvm->DetachCurrentThread();
        }
    }
}

static void udata_dtor_tagged(lua_State* L, void* p) {
    jobject ref = *(jobject*)p;
    release_global_ref(ref);
}

static void udata_dtor_custom(void* p) {
    jobject ref = *(jobject*)p;
    release_global_ref(ref);
}

static int jni_callback_trampoline(lua_State* L) {
    jobject* pRef = (jobject*) lua_touserdata(L, lua_upvalueindex(1));
    if (!pRef || !*pRef) {
        lua_pushstring(L, "Internal error: callback object is null");
        lua_error(L);
        return 0;
    }
    jobject callback_obj = *pRef;

    JNIEnv* env = nullptr;
    g_jvm->GetEnv((void**)&env, JNI_VERSION_1_6);

    jobject state_obj = env->NewObject(g_luaStateCls, g_luaStateCtor, (jlong)L, false);

    jint result = env->CallIntMethod(callback_obj, g_luaFuncInvoke, state_obj);

    if (env->ExceptionCheck()) {
        jthrowable exc = env->ExceptionOccurred();
        env->ExceptionClear();
        g_pendingException = env->NewGlobalRef(exc);
        return -100 - 3; // -103 (LUA_ERRRUN)
    }

    return result;
}

static int jni_continuation_trampoline(lua_State* L, int status) {
    jobject* pRef = (jobject*) lua_touserdata(L, lua_upvalueindex(1));
    if (!pRef || !*pRef) {
        lua_pushstring(L, "Internal error: callback object is null");
        lua_error(L);
        return 0;
    }
    jobject callback_obj = *pRef;

    JNIEnv* env = nullptr;
    g_jvm->GetEnv((void**)&env, JNI_VERSION_1_6);

    jobject state_obj = env->NewObject(g_luaStateCls, g_luaStateCtor, (jlong)L, false);

    jint result = env->CallIntMethod(callback_obj, g_luaYieldableFuncResume, state_obj, status);

    if (env->ExceptionCheck()) {
        jthrowable exc = env->ExceptionOccurred();
        env->ExceptionClear();
        g_pendingException = env->NewGlobalRef(exc);
        return -100 - 3; // -103 (LUA_ERRRUN)
    }

    return result;
}

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    g_jvm = vm;
    JNIEnv* env = nullptr;
    if (vm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }

    jclass cls = env->FindClass("io/github/novela/luau/LuaState");
    g_luaStateCls = (jclass)env->NewGlobalRef(cls);
    g_luaStateCtor = env->GetMethodID(g_luaStateCls, "<init>", "(JZ)V");

    cls = env->FindClass("io/github/novela/luau/LuaFunction");
    g_luaFuncCls = (jclass)env->NewGlobalRef(cls);
    g_luaFuncInvoke = env->GetMethodID(g_luaFuncCls, "invoke", "(Lio/github/novela/luau/LuaState;)I");

    cls = env->FindClass("io/github/novela/luau/LuaYieldableFunction");
    g_luaYieldableFuncCls = (jclass)env->NewGlobalRef(cls);
    g_luaYieldableFuncResume = env->GetMethodID(g_luaYieldableFuncCls, "resume", "(Lio/github/novela/luau/LuaState;I)I");

    return JNI_VERSION_1_6;
}

// LuaEngine native implementations
extern "C" JNIEXPORT jlong JNICALL Java_io_github_novela_luau_LuaEngine_nativeNewState(JNIEnv* env, jclass) {
    lua_State* L = luaW_newstate(nullptr);
    if (!L) return 0L;

    // Register user data destructor for all tags
    for (int i = 0; i < LUA_UTAG_LIMIT; i++) {
        lua_setuserdatadtor(L, i, udata_dtor_tagged);
    }

    return reinterpret_cast<jlong>(L);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_novela_luau_LuaEngine_nativeOpenLibs(JNIEnv* env, jclass, jlong statePtr) {
    lua_State* L = reinterpret_cast<lua_State*>(statePtr);
    luaopen_base(L);
    luaopen_string(L);
    luaopen_table(L);
    luaopen_math(L);
    luaopen_utf8(L);
    luaopen_buffer(L);
    luaopen_coroutine(L);
}

extern "C" JNIEXPORT jlong JNICALL Java_io_github_novela_luau_LuaEngine_nativeCompile(JNIEnv* env, jclass, jbyteArray source, jint length, jlongArray sizeOut) {
    jbyte* src = env->GetByteArrayElements(source, nullptr);

    lua_CompileOptions options = {};
    options.optimizationLevel = 1; // baseline
    options.debugLevel = 1;        // line info
    options.typeInfoLevel = 0;
    options.coverageLevel = 0;

    size_t outSize = 0;
    char* bytecode = luau_compile(reinterpret_cast<const char*>(src), length, &options, &outSize);

    env->ReleaseByteArrayElements(source, src, JNI_ABORT);

    jlong sizeVal = outSize;
    env->SetLongArrayRegion(sizeOut, 0, 1, &sizeVal);

    return reinterpret_cast<jlong>(bytecode);
}

extern "C" JNIEXPORT jbyteArray JNICALL Java_io_github_novela_luau_LuaEngine_nativeReadBytesAndFree(JNIEnv* env, jclass, jlong resultPtr, jint size) {
    char* bytecode = reinterpret_cast<char*>(resultPtr);
    if (!bytecode) return nullptr;

    jbyteArray arr = env->NewByteArray(size);
    env->SetByteArrayRegion(arr, 0, size, reinterpret_cast<jbyte*>(bytecode));
    luau_ext_free(bytecode);

    return arr;
}

// LuaState native lifecycle
extern "C" JNIEXPORT void JNICALL Java_io_github_novela_luau_LuaState_nativeClose(JNIEnv*, jclass, jlong statePtr) {
    lua_close(reinterpret_cast<lua_State*>(statePtr));
}

extern "C" JNIEXPORT jlong JNICALL Java_io_github_novela_luau_LuaState_nativeNewThread(JNIEnv*, jclass, jlong statePtr) {
    lua_State* L = reinterpret_cast<lua_State*>(statePtr);
    lua_State* NL = luaW_newthread(L);
    return reinterpret_cast<jlong>(NL);
}

// LuaState native stack operations
extern "C" JNIEXPORT jint JNICALL Java_io_github_novela_luau_LuaState_nativeGetTop(JNIEnv*, jclass, jlong statePtr) {
    return lua_gettop(reinterpret_cast<lua_State*>(statePtr));
}

extern "C" JNIEXPORT void JNICALL Java_io_github_novela_luau_LuaState_nativeSetTop(JNIEnv*, jclass, jlong statePtr, jint idx) {
    lua_settop(reinterpret_cast<lua_State*>(statePtr), idx);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_novela_luau_LuaState_nativePushValue(JNIEnv*, jclass, jlong statePtr, jint idx) {
    lua_pushvalue(reinterpret_cast<lua_State*>(statePtr), idx);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_novela_luau_LuaState_nativeRemove(JNIEnv*, jclass, jlong statePtr, jint idx) {
    lua_remove(reinterpret_cast<lua_State*>(statePtr), idx);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_novela_luau_LuaState_nativeInsert(JNIEnv*, jclass, jlong statePtr, jint idx) {
    lua_insert(reinterpret_cast<lua_State*>(statePtr), idx);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_novela_luau_LuaState_nativeReplace(JNIEnv*, jclass, jlong statePtr, jint idx) {
    lua_replace(reinterpret_cast<lua_State*>(statePtr), idx);
}

extern "C" JNIEXPORT jint JNICALL Java_io_github_novela_luau_LuaState_nativeAbsIndex(JNIEnv*, jclass, jlong statePtr, jint idx) {
    return lua_absindex(reinterpret_cast<lua_State*>(statePtr), idx);
}

// LuaState native pushing
extern "C" JNIEXPORT void JNICALL Java_io_github_novela_luau_LuaState_nativePushNil(JNIEnv*, jclass, jlong statePtr) {
    lua_pushnil(reinterpret_cast<lua_State*>(statePtr));
}

extern "C" JNIEXPORT void JNICALL Java_io_github_novela_luau_LuaState_nativePushBoolean(JNIEnv*, jclass, jlong statePtr, jboolean b) {
    lua_pushboolean(reinterpret_cast<lua_State*>(statePtr), b ? 1 : 0);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_novela_luau_LuaState_nativePushNumber(JNIEnv*, jclass, jlong statePtr, jdouble n) {
    lua_pushnumber(reinterpret_cast<lua_State*>(statePtr), n);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_novela_luau_LuaState_nativePushInteger(JNIEnv*, jclass, jlong statePtr, jlong i) {
    lua_pushinteger64(reinterpret_cast<lua_State*>(statePtr), i);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_novela_luau_LuaState_nativePushString(JNIEnv* env, jclass, jlong statePtr, jstring s) {
    lua_State* L = reinterpret_cast<lua_State*>(statePtr);
    const char* str = env->GetStringUTFChars(s, nullptr);
    jsize len = env->GetStringUTFLength(s);
    luaW_pushlstring(L, str, len);
    env->ReleaseStringUTFChars(s, str);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_novela_luau_LuaState_nativePushVector(JNIEnv*, jclass, jlong statePtr, jfloat x, jfloat y, jfloat z) {
    lua_pushvector(reinterpret_cast<lua_State*>(statePtr), x, y, z);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_novela_luau_LuaState_nativePushLightUserData(JNIEnv*, jclass, jlong statePtr, jlong ptr) {
    lua_pushlightuserdatatagged(reinterpret_cast<lua_State*>(statePtr), reinterpret_cast<void*>(ptr), 0);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_novela_luau_LuaState_nativeNewUserData(JNIEnv* env, jclass, jlong statePtr, jobject value, jint tag) {
    lua_State* L = reinterpret_cast<lua_State*>(statePtr);
    jobject* pRef = (jobject*) luaW_newuserdatatagged(L, sizeof(jobject), tag);
    *pRef = env->NewGlobalRef(value);
}

extern "C" JNIEXPORT jobject JNICALL Java_io_github_novela_luau_LuaState_nativeNewBuffer(JNIEnv* env, jclass, jlong statePtr, jlong size) {
    lua_State* L = reinterpret_cast<lua_State*>(statePtr);
    void* ptr = luaW_newbuffer(L, size);
    return ptr ? env->NewDirectByteBuffer(ptr, size) : nullptr;
}

// LuaState native types & inspection
extern "C" JNIEXPORT jint JNICALL Java_io_github_novela_luau_LuaState_nativeType(JNIEnv*, jclass, jlong statePtr, jint idx) {
    return lua_type(reinterpret_cast<lua_State*>(statePtr), idx);
}

extern "C" JNIEXPORT jstring JNICALL Java_io_github_novela_luau_LuaState_nativeTypeName(JNIEnv* env, jclass, jlong statePtr, jint idx) {
    lua_State* L = reinterpret_cast<lua_State*>(statePtr);
    const char* name = luaLW_typename(L, idx);
    return name ? env->NewStringUTF(name) : nullptr;
}

extern "C" JNIEXPORT jboolean JNICALL Java_io_github_novela_luau_LuaState_nativeToBoolean(JNIEnv*, jclass, jlong statePtr, jint idx) {
    return lua_toboolean(reinterpret_cast<lua_State*>(statePtr), idx) != 0;
}

extern "C" JNIEXPORT jdouble JNICALL Java_io_github_novela_luau_LuaState_nativeToNumber(JNIEnv*, jclass, jlong statePtr, jint idx) {
    return lua_tonumberx(reinterpret_cast<lua_State*>(statePtr), idx, nullptr);
}

extern "C" JNIEXPORT jlong JNICALL Java_io_github_novela_luau_LuaState_nativeToInteger(JNIEnv*, jclass, jlong statePtr, jint idx) {
    return lua_tointeger64(reinterpret_cast<lua_State*>(statePtr), idx, nullptr);
}

extern "C" JNIEXPORT jstring JNICALL Java_io_github_novela_luau_LuaState_nativeToString(JNIEnv* env, jclass, jlong statePtr, jint idx) {
    lua_State* L = reinterpret_cast<lua_State*>(statePtr);
    size_t len = 0;
    const char* str = luaW_tolstring(L, idx, &len);
    return str ? env->NewStringUTF(str) : nullptr;
}

extern "C" JNIEXPORT jfloatArray JNICALL Java_io_github_novela_luau_LuaState_nativeToVector(JNIEnv* env, jclass, jlong statePtr, jint idx) {
    lua_State* L = reinterpret_cast<lua_State*>(statePtr);
    const float* v = lua_tovector(L, idx);
    jfloatArray arr = env->NewFloatArray(3);
    if (v) {
        env->SetFloatArrayRegion(arr, 0, 3, v);
    } else {
        float empty[3] = {0, 0, 0};
        env->SetFloatArrayRegion(arr, 0, 3, empty);
    }
    return arr;
}

extern "C" JNIEXPORT jobject JNICALL Java_io_github_novela_luau_LuaState_nativeToUserdata(JNIEnv*, jclass, jlong statePtr, jint idx) {
    lua_State* L = reinterpret_cast<lua_State*>(statePtr);
    jobject* pRef = (jobject*) lua_touserdata(L, idx);
    return (pRef && *pRef) ? *pRef : nullptr;
}

extern "C" JNIEXPORT jint JNICALL Java_io_github_novela_luau_LuaState_nativeUserDataTag(JNIEnv*, jclass, jlong statePtr, jint idx) {
    return lua_userdatatag(reinterpret_cast<lua_State*>(statePtr), idx);
}

extern "C" JNIEXPORT jobject JNICALL Java_io_github_novela_luau_LuaState_nativeToBuffer(JNIEnv* env, jclass, jlong statePtr, jint idx) {
    lua_State* L = reinterpret_cast<lua_State*>(statePtr);
    size_t len = 0;
    void* ptr = lua_tobuffer(L, idx, &len);
    return ptr ? env->NewDirectByteBuffer(ptr, len) : nullptr;
}

// Comparisons
extern "C" JNIEXPORT jboolean JNICALL Java_io_github_novela_luau_LuaState_nativeEqual(JNIEnv*, jclass, jlong statePtr, jint idx1, jint idx2) {
    lua_State* L = reinterpret_cast<lua_State*>(statePtr);
    return luaW_equal(L, idx1, idx2) != 0;
}

extern "C" JNIEXPORT jboolean JNICALL Java_io_github_novela_luau_LuaState_nativeRawEqual(JNIEnv*, jclass, jlong statePtr, jint idx1, jint idx2) {
    return lua_rawequal(reinterpret_cast<lua_State*>(statePtr), idx1, idx2) != 0;
}

extern "C" JNIEXPORT jboolean JNICALL Java_io_github_novela_luau_LuaState_nativeLessThan(JNIEnv*, jclass, jlong statePtr, jint idx1, jint idx2) {
    lua_State* L = reinterpret_cast<lua_State*>(statePtr);
    return luaW_lessthan(L, idx1, idx2) != 0;
}

extern "C" JNIEXPORT jint JNICALL Java_io_github_novela_luau_LuaState_nativeObjLen(JNIEnv*, jclass, jlong statePtr, jint idx) {
    lua_State* L = reinterpret_cast<lua_State*>(statePtr);
    return luaW_objlen(L, idx);
}

// Tables
extern "C" JNIEXPORT void JNICALL Java_io_github_novela_luau_LuaState_nativeCreateTable(JNIEnv*, jclass, jlong statePtr, jint narr, jint nrec) {
    lua_State* L = reinterpret_cast<lua_State*>(statePtr);
    luaW_createtable(L, narr, nrec);
}

extern "C" JNIEXPORT jint JNICALL Java_io_github_novela_luau_LuaState_nativeGetTable(JNIEnv*, jclass, jlong statePtr, jint idx) {
    lua_State* L = reinterpret_cast<lua_State*>(statePtr);
    return luaW_gettable(L, idx);
}

extern "C" JNIEXPORT jint JNICALL Java_io_github_novela_luau_LuaState_nativeGetField(JNIEnv* env, jclass, jlong statePtr, jint idx, jstring k) {
    lua_State* L = reinterpret_cast<lua_State*>(statePtr);
    const char* key = env->GetStringUTFChars(k, nullptr);
    int t = luaW_getfield(L, idx, key);
    env->ReleaseStringUTFChars(k, key);
    return t;
}

extern "C" JNIEXPORT jint JNICALL Java_io_github_novela_luau_LuaState_nativeRawGet(JNIEnv*, jclass, jlong statePtr, jint idx) {
    return lua_rawget(reinterpret_cast<lua_State*>(statePtr), idx);
}

extern "C" JNIEXPORT jint JNICALL Java_io_github_novela_luau_LuaState_nativeRawGetI(JNIEnv*, jclass, jlong statePtr, jint idx, jint n) {
    return lua_rawgeti(reinterpret_cast<lua_State*>(statePtr), idx, n);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_novela_luau_LuaState_nativeSetTable(JNIEnv*, jclass, jlong statePtr, jint idx) {
    lua_State* L = reinterpret_cast<lua_State*>(statePtr);
    luaW_settable(L, idx);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_novela_luau_LuaState_nativeSetField(JNIEnv* env, jclass, jlong statePtr, jint idx, jstring k) {
    lua_State* L = reinterpret_cast<lua_State*>(statePtr);
    const char* key = env->GetStringUTFChars(k, nullptr);
    luaW_setfield(L, idx, key);
    env->ReleaseStringUTFChars(k, key);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_novela_luau_LuaState_nativeRawSet(JNIEnv*, jclass, jlong statePtr, jint idx) {
    lua_State* L = reinterpret_cast<lua_State*>(statePtr);
    luaW_rawset(L, idx);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_novela_luau_LuaState_nativeRawSetI(JNIEnv*, jclass, jlong statePtr, jint idx, jint n) {
    lua_State* L = reinterpret_cast<lua_State*>(statePtr);
    luaW_rawseti(L, idx, n);
}

extern "C" JNIEXPORT jboolean JNICALL Java_io_github_novela_luau_LuaState_nativeGetReadOnly(JNIEnv*, jclass, jlong statePtr, jint idx) {
    return lua_getreadonly(reinterpret_cast<lua_State*>(statePtr), idx) != 0;
}

extern "C" JNIEXPORT void JNICALL Java_io_github_novela_luau_LuaState_nativeSetReadOnly(JNIEnv*, jclass, jlong statePtr, jint idx, jboolean enabled) {
    lua_setreadonly(reinterpret_cast<lua_State*>(statePtr), idx, enabled ? 1 : 0);
}

extern "C" JNIEXPORT jboolean JNICALL Java_io_github_novela_luau_LuaState_nativeGetMetaTable(JNIEnv*, jclass, jlong statePtr, jint idx) {
    return lua_getmetatable(reinterpret_cast<lua_State*>(statePtr), idx) != 0;
}

extern "C" JNIEXPORT void JNICALL Java_io_github_novela_luau_LuaState_nativeSetMetaTable(JNIEnv*, jclass, jlong statePtr, jint idx) {
    lua_State* L = reinterpret_cast<lua_State*>(statePtr);
    luaW_setmetatable(L, idx);
}

// Callback registration
extern "C" JNIEXPORT void JNICALL Java_io_github_novela_luau_LuaState_nativePushFunction(JNIEnv* env, jclass, jlong statePtr, jobject callback, jstring debugName) {
    lua_State* L = reinterpret_cast<lua_State*>(statePtr);
    const char* dname = env->GetStringUTFChars(debugName, nullptr);

    jobject* pRef = (jobject*) lua_newuserdatadtor(L, sizeof(jobject), udata_dtor_custom);
    *pRef = env->NewGlobalRef(callback);

    lua_pushcclosurek(L, jni_callback_trampoline, dname, 1, nullptr);

    env->ReleaseStringUTFChars(debugName, dname);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_novela_luau_LuaState_nativePushYieldableFunction(JNIEnv* env, jclass, jlong statePtr, jobject callback, jstring debugName) {
    lua_State* L = reinterpret_cast<lua_State*>(statePtr);
    const char* dname = env->GetStringUTFChars(debugName, nullptr);

    jobject* pRef = (jobject*) lua_newuserdatadtor(L, sizeof(jobject), udata_dtor_custom);
    *pRef = env->NewGlobalRef(callback);

    lua_pushcclosurek(L, jni_callback_trampoline, dname, 1, jni_continuation_trampoline);

    env->ReleaseStringUTFChars(debugName, dname);
}

// Registry helpers
extern "C" JNIEXPORT jint JNICALL Java_io_github_novela_luau_LuaState_nativeRef(JNIEnv*, jclass, jlong statePtr, jint idx) {
    return lua_ref(reinterpret_cast<lua_State*>(statePtr), idx);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_novela_luau_LuaState_nativeGetRef(JNIEnv*, jclass, jlong statePtr, jint refId) {
    lua_rawgeti(reinterpret_cast<lua_State*>(statePtr), LUA_REGISTRYINDEX, refId);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_novela_luau_LuaState_nativeUnref(JNIEnv*, jclass, jlong statePtr, jint refId) {
    lua_unref(reinterpret_cast<lua_State*>(statePtr), refId);
}

// Call, Load, Yield, Resume
extern "C" JNIEXPORT jint JNICALL Java_io_github_novela_luau_LuaState_nativeLoad(JNIEnv* env, jclass, jlong statePtr, jstring chunkName, jbyteArray bytecode) {
    lua_State* L = reinterpret_cast<lua_State*>(statePtr);
    const char* name = env->GetStringUTFChars(chunkName, nullptr);
    jbyte* bytes = env->GetByteArrayElements(bytecode, nullptr);
    jsize len = env->GetArrayLength(bytecode);

    int res = luau_load(L, name, reinterpret_cast<const char*>(bytes), len, 0);

    env->ReleaseByteArrayElements(bytecode, bytes, JNI_ABORT);
    env->ReleaseStringUTFChars(chunkName, name);
    return res;
}

static int error_handler(lua_State* L) {
    const char* msg = lua_tostring(L, 1);
    if (!msg) msg = "";
    luaL_traceback(L, L, msg, 1);
    return 1;
}

extern "C" JNIEXPORT jint JNICALL Java_io_github_novela_luau_LuaState_nativeCall(JNIEnv*, jclass, jlong statePtr, jint nargs, jint nresults) {
    lua_State* L = reinterpret_cast<lua_State*>(statePtr);
    
    // Push error handler onto the stack
    lua_pushcfunction(L, error_handler, "error_handler");
    
    // Insert error handler below the function and arguments
    int errfunc = lua_gettop(L) - nargs - 1;
    lua_insert(L, errfunc);
    
    // Call lua_pcall
    int status = lua_pcall(L, nargs, nresults, errfunc);
    
    // Remove error handler from the stack
    lua_remove(L, errfunc);
    
    return status;
}

extern "C" JNIEXPORT jint JNICALL Java_io_github_novela_luau_LuaState_nativeYield(JNIEnv*, jclass, jlong statePtr, jint nresults) {
    lua_State* L = reinterpret_cast<lua_State*>(statePtr);
    return luaW_yield(L, nresults);
}

extern "C" JNIEXPORT jint JNICALL Java_io_github_novela_luau_LuaState_nativeResume(JNIEnv*, jclass, jlong statePtr, jlong fromPtr, jint nargs) {
    lua_State* L = reinterpret_cast<lua_State*>(statePtr);
    lua_State* FL = reinterpret_cast<lua_State*>(fromPtr);
    return lua_resume(L, FL, nargs);
}

extern "C" JNIEXPORT jint JNICALL Java_io_github_novela_luau_LuaState_nativeStatus(JNIEnv*, jclass, jlong statePtr) {
    return lua_status(reinterpret_cast<lua_State*>(statePtr));
}

extern "C" JNIEXPORT jboolean JNICALL Java_io_github_novela_luau_LuaState_nativeIsYieldable(JNIEnv*, jclass, jlong statePtr) {
    return lua_isyieldable(reinterpret_cast<lua_State*>(statePtr)) != 0;
}

// GC & Memory
extern "C" JNIEXPORT jint JNICALL Java_io_github_novela_luau_LuaState_nativeGc(JNIEnv*, jclass, jlong statePtr, jint op, jint data) {
    lua_State* L = reinterpret_cast<lua_State*>(statePtr);
    return lua_gc(L, op, data);
}

extern "C" JNIEXPORT jlong JNICALL Java_io_github_novela_luau_LuaState_nativeTotalBytes(JNIEnv*, jclass, jlong statePtr, jint category) {
    return lua_totalbytes(reinterpret_cast<lua_State*>(statePtr), category);
}

// Sandboxing
extern "C" JNIEXPORT void JNICALL Java_io_github_novela_luau_LuaState_nativeSandbox(JNIEnv*, jclass, jlong statePtr, jlong threadPtr) {
    luaL_sandboxthread(reinterpret_cast<lua_State*>(threadPtr));
}

extern "C" JNIEXPORT void JNICALL Java_io_github_novela_luau_LuaState_nativeSandboxThread(JNIEnv*, jclass, jlong statePtr) {
    luaL_sandboxthread(reinterpret_cast<lua_State*>(statePtr));
}

// Error status checks
extern "C" JNIEXPORT jint JNICALL Java_io_github_novela_luau_LuaState_nativeGetStatus(JNIEnv* env, jclass, jlong statePtr) {
    lua_State* L = reinterpret_cast<lua_State*>(statePtr);
    if (g_pendingException) {
        jobject exc = g_pendingException;
        g_pendingException = nullptr;
        env->Throw((jthrowable)exc);
        env->DeleteGlobalRef(exc);
        return 0;
    }
    return luaW_getstatus(L);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_novela_luau_LuaState_nativePop(JNIEnv*, jclass, jlong statePtr, jint n) {
    lua_State* L = reinterpret_cast<lua_State*>(statePtr);
    lua_settop(L, -n - 1);
}
