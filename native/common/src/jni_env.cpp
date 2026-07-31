#include "novelua/common/jni_env.hpp"
#include <pthread.h>
#include <stdexcept>
#include <cassert>

namespace novelua::common {

namespace {

JavaVM* g_javaVM = nullptr;
pthread_key_t g_threadKey;
bool g_keyInitialized = false;

void DetachCurrentThreadCallback(void* env) {
    if (env != nullptr && g_javaVM != nullptr) {
        g_javaVM->DetachCurrentThread();
    }
}

void InitThreadKey() {
    if (!g_keyInitialized) {
        pthread_key_create(&g_threadKey, DetachCurrentThreadCallback);
        g_keyInitialized = true;
    }
}

} // namespace

void SetJavaVM(JavaVM* vm) {
    g_javaVM = vm;
    InitThreadKey();
}

JavaVM* GetJavaVM() {
    return g_javaVM;
}

JNIEnv* GetJNIEnv() {
    assert(g_javaVM != nullptr && "JavaVM not initialized");
    JNIEnv* env = nullptr;
    jint getEnvStat = g_javaVM->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);

    if (getEnvStat == JNI_EDETACHED) {
        if (g_javaVM->AttachCurrentThread(&env, nullptr) != JNI_OK) {
            throw std::runtime_error("Failed to attach thread to JVM");
        }
        pthread_setspecific(g_threadKey, env);
    } else if (getEnvStat != JNI_OK) {
        throw std::runtime_error("Failed to get JNIEnv");
    }

    return env;
}

} // namespace novelua::common
