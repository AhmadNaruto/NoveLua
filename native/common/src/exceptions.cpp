#include "novelua/common/exceptions.hpp"

namespace novelua::common {

namespace {

void ThrowException(JNIEnv* env, const char* className, const char* message) {
    if (env->ExceptionCheck()) {
        // Already an exception pending, don't overwrite it.
        return;
    }
    jclass exClass = env->FindClass(className);
    if (exClass != nullptr) {
        env->ThrowNew(exClass, message);
        env->DeleteLocalRef(exClass);
    }
}

} // namespace

void ThrowRuntimeException(JNIEnv* env, const char* message) {
    ThrowException(env, "java/lang/RuntimeException", message);
}

void ThrowIllegalArgumentException(JNIEnv* env, const char* message) {
    ThrowException(env, "java/lang/IllegalArgumentException", message);
}

void ThrowIllegalStateException(JNIEnv* env, const char* message) {
    ThrowException(env, "java/lang/IllegalStateException", message);
}

bool CheckAndClearException(JNIEnv* env) {
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        return true;
    }
    return false;
}

} // namespace novelua::common
