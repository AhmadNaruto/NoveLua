#include <jni.h>
#include "novelua/regex/pcre2_engine.hpp"
#include "novelua/common/jstring_utils.hpp"
#include "novelua/common/exceptions.hpp"
#include <string>

using namespace novelua::regex;
using namespace novelua::common;

extern "C" {

static jlong Pattern_create(JNIEnv* env, jobject, jstring jpattern, jint options) {
    try {
        std::string pattern = ToStdString(env, jpattern);
        auto* engine = new Pcre2Engine(pattern, static_cast<uint32_t>(options));
        return reinterpret_cast<jlong>(engine);
    } catch (const std::invalid_argument& e) {
        ThrowIllegalArgumentException(env, e.what());
        return 0;
    } catch (const std::exception& e) {
        ThrowRuntimeException(env, e.what());
        return 0;
    }
}

static void Pattern_destroy(JNIEnv*, jobject, jlong handle) {
    auto* engine = reinterpret_cast<Pcre2Engine*>(handle);
    delete engine;
}

static jobject Matcher_match(JNIEnv* env, jobject, jlong handle, jstring jsubject, jint offset) {
    auto* engine = reinterpret_cast<Pcre2Engine*>(handle);
    std::string subject = ToStdString(env, jsubject);
    
    MatchResult result = engine->match(subject, static_cast<size_t>(offset));
    
    if (!result.matched) {
        return nullptr;
    }
    
    jclass matchClass = env->FindClass("io/github/novelua/regex/Match");
    jmethodID matchConstructor = env->GetMethodID(matchClass, "<init>", "(Ljava/lang/String;II[Lio/github/novelua/regex/Group;)V");
    
    jclass groupClass = env->FindClass("io/github/novelua/regex/Group");
    jmethodID groupConstructor = env->GetMethodID(groupClass, "<init>", "(ILjava/lang/String;)V");
    
    jobjectArray groupsArray = env->NewObjectArray(result.groups.size(), groupClass, nullptr);
    for (size_t i = 0; i < result.groups.size(); ++i) {
        jstring groupValue = env->NewStringUTF(result.groups[i].value.c_str());
        jobject groupObj = env->NewObject(groupClass, groupConstructor, result.groups[i].index, groupValue);
        env->SetObjectArrayElement(groupsArray, i, groupObj);
        env->DeleteLocalRef(groupValue);
        env->DeleteLocalRef(groupObj);
    }
    
    jstring matchValue = env->NewStringUTF(subject.substr(result.start, result.end - result.start).c_str());
    jobject matchObj = env->NewObject(matchClass, matchConstructor, matchValue, result.start, result.end, groupsArray);
    
    return matchObj;
}

static jstring Pattern_replaceFirst(JNIEnv* env, jobject, jlong handle, jstring jsubject, jstring jreplacement) {
    auto* engine = reinterpret_cast<Pcre2Engine*>(handle);
    std::string subject = ToStdString(env, jsubject);
    std::string replacement = ToStdString(env, jreplacement);
    try {
        std::string result = engine->replaceFirst(subject, replacement);
        return env->NewStringUTF(result.c_str());
    } catch (const std::exception& e) {
        ThrowRuntimeException(env, e.what());
        return nullptr;
    }
}

static jstring Pattern_replaceAll(JNIEnv* env, jobject, jlong handle, jstring jsubject, jstring jreplacement) {
    auto* engine = reinterpret_cast<Pcre2Engine*>(handle);
    std::string subject = ToStdString(env, jsubject);
    std::string replacement = ToStdString(env, jreplacement);
    try {
        std::string result = engine->replaceAll(subject, replacement);
        return env->NewStringUTF(result.c_str());
    } catch (const std::exception& e) {
        ThrowRuntimeException(env, e.what());
        return nullptr;
    }
}

static const JNINativeMethod gPatternMethods[] = {
    {"nativeCreate", "(Ljava/lang/String;I)J", reinterpret_cast<void*>(Pattern_create)},
    {"nativeDestroy", "(J)V", reinterpret_cast<void*>(Pattern_destroy)},
    {"nativeReplaceFirst", "(JLjava/lang/String;Ljava/lang/String;)Ljava/lang/String;", reinterpret_cast<void*>(Pattern_replaceFirst)},
    {"nativeReplaceAll", "(JLjava/lang/String;Ljava/lang/String;)Ljava/lang/String;", reinterpret_cast<void*>(Pattern_replaceAll)}
};

static const JNINativeMethod gMatcherMethods[] = {
    {"nativeMatch", "(JLjava/lang/String;I)Lio/github/novelua/regex/Match;", reinterpret_cast<void*>(Matcher_match)}
};

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }

    jclass patternClass = env->FindClass("io/github/novelua/regex/Pattern");
    if (env->RegisterNatives(patternClass, gPatternMethods, sizeof(gPatternMethods) / sizeof(gPatternMethods[0])) < 0) {
        return JNI_ERR;
    }

    jclass matcherClass = env->FindClass("io/github/novelua/regex/Matcher");
    if (env->RegisterNatives(matcherClass, gMatcherMethods, sizeof(gMatcherMethods) / sizeof(gMatcherMethods[0])) < 0) {
        return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}

} // extern "C"
