#pragma once

#include <jni.h>

namespace novelua::common {

/**
 * Throws a java/lang/RuntimeException.
 */
void ThrowRuntimeException(JNIEnv* env, const char* message);

/**
 * Throws a java/lang/IllegalArgumentException.
 */
void ThrowIllegalArgumentException(JNIEnv* env, const char* message);

/**
 * Throws a java/lang/IllegalStateException.
 */
void ThrowIllegalStateException(JNIEnv* env, const char* message);

/**
 * Checks for a pending JNI exception, clears it, and returns true if an exception was pending.
 * Useful for swallowing exceptions or handling them natively.
 */
bool CheckAndClearException(JNIEnv* env);

} // namespace novelua::common
