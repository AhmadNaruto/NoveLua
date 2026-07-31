import os

def create_file(path, content):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, 'w') as f:
        f.write(content)

src_dir = "native/lexsoup/src"

jni_cpp = """#include <jni.h>
#include "lexsoup/document.hpp"
#include "lexsoup/element.hpp"
#include "lexsoup/selector.hpp"
#include <novelua/common/jni_env.hpp>
#include <novelua/common/jstring_utils.hpp>

using namespace novelua::lexsoup;
using namespace novelua::common;

extern "C" JNIEXPORT jlong JNICALL
Document_create(JNIEnv* env, jobject) {
    return reinterpret_cast<jlong>(new Document());
}

extern "C" JNIEXPORT void JNICALL
Document_destroy(JNIEnv* env, jobject, jlong handle) {
    delete reinterpret_cast<Document*>(handle);
}

extern "C" JNIEXPORT jboolean JNICALL
Document_parse(JNIEnv* env, jobject, jlong handle, jstring html) {
    auto* doc = reinterpret_cast<Document*>(handle);
    JString str(env, html);
    return doc->Parse(str.ToString()) ? JNI_TRUE : JNI_FALSE;
}

// ... other JNI wrappers ...

static const JNINativeMethod g_document_methods[] = {
    {"nativeCreate", "()J", (void*)Document_create},
    {"nativeDestroy", "(J)V", (void*)Document_destroy},
    {"nativeParse", "(JLjava/lang/String;)Z", (void*)Document_parse}
};

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }

    jclass docClass = env->FindClass("io/github/novelua/lexsoup/Document");
    if (docClass == nullptr) return JNI_ERR;
    
    if (env->RegisterNatives(docClass, g_document_methods, 
        sizeof(g_document_methods) / sizeof(g_document_methods[0])) < 0) {
        return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}
"""

create_file(f"{src_dir}/lexsoup_jni.cpp", jni_cpp)
