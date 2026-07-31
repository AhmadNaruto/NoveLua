#include <jni.h>
#include "../common/common.h"

namespace novelua::luau {

static const JNINativeMethod gMethods[] = {
    // Methods to be registered
};

} // namespace novelua::luau

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* /*reserved*/) {
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    // Register methods here
    return JNI_VERSION_1_6;
}
