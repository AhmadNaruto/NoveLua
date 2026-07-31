#pragma once

#include <jni.h>

namespace novelua::common {

/**
 * Stores the global JavaVM instance.
 * Must be called in JNI_OnLoad.
 */
void SetJavaVM(JavaVM* vm);

/**
 * Returns the global JavaVM instance.
 */
JavaVM* GetJavaVM();

/**
 * Retrieves the JNIEnv for the current thread.
 * Automatically attaches the thread to the JVM if necessary, and handles detaching on thread exit.
 */
JNIEnv* GetJNIEnv();

} // namespace novelua::common
