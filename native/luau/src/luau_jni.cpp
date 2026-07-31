#include <jni.h>
#include "vm_wrapper.hpp"
#include <novelua/common/jstring_utils.hpp>
#include <novelua/common/exceptions.hpp>

using namespace novelua::luau;
using namespace novelua::common;

extern "C" {

static jlong VM_create(JNIEnv* env, jobject) {
    try {
        return reinterpret_cast<jlong>(new VM());
    } catch (const std::exception& e) {
        ThrowRuntimeException(env, e.what());
        return 0;
    }
}

static void VM_destroy(JNIEnv*, jobject, jlong handle) {
    auto* vm = reinterpret_cast<VM*>(handle);
    delete vm;
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

static const JNINativeMethod gVMMethods[] = {
    {"nativeCreate", "()J", reinterpret_cast<void*>(VM_create)},
    {"nativeDestroy", "(J)V", reinterpret_cast<void*>(VM_destroy)},
    {"nativeEval", "(JLjava/lang/String;)Ljava/lang/String;", reinterpret_cast<void*>(VM_eval)}
};

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }

    jclass vmClass = env->FindClass("io/github/novelua/luau/VM");
    if (vmClass && env->RegisterNatives(vmClass, gVMMethods, sizeof(gVMMethods) / sizeof(gVMMethods[0])) >= 0) {
        return JNI_VERSION_1_6;
    }

    return JNI_VERSION_1_6;
}

} // extern "C"
