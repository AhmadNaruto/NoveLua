#include <jni.h>
#include "lexsoup/document.hpp"
#include "lexsoup/element.hpp"
#include "lexsoup/selector.hpp"
#include <novelua/common/jni_env.hpp>
#include <novelua/common/jstring_utils.hpp>
#include <novelua/common/exceptions.hpp>

using namespace novelua::lexsoup;
using namespace novelua::common;

extern "C" {

static jlong Document_create(JNIEnv* env, jobject) {
    try {
        return reinterpret_cast<jlong>(new Document());
    } catch (const std::exception& e) {
        ThrowRuntimeException(env, e.what());
        return 0;
    }
}

static void Document_destroy(JNIEnv*, jobject, jlong handle) {
    if (handle != 0) {
        delete reinterpret_cast<Document*>(handle);
    }
}

static jboolean Document_parse(JNIEnv* env, jobject, jlong handle, jstring html) {
    auto* doc = reinterpret_cast<Document*>(handle);
    if (!doc) return JNI_FALSE;
    JString str(env, html);
    return doc->Parse(str.ToString()) ? JNI_TRUE : JNI_FALSE;
}

static jstring Document_title(JNIEnv* env, jobject, jlong handle) {
    auto* doc = reinterpret_cast<Document*>(handle);
    if (!doc) return env->NewStringUTF("");
    return env->NewStringUTF(doc->Title().c_str());
}

static jlong Document_head(JNIEnv*, jobject, jlong handle) {
    auto* doc = reinterpret_cast<Document*>(handle);
    if (!doc) return 0;
    return reinterpret_cast<jlong>(new Element(doc->Head()));
}

static jlong Document_body(JNIEnv*, jobject, jlong handle) {
    auto* doc = reinterpret_cast<Document*>(handle);
    if (!doc) return 0;
    return reinterpret_cast<jlong>(new Element(doc->Body()));
}

static jstring Document_html(JNIEnv* env, jobject, jlong handle) {
    auto* doc = reinterpret_cast<Document*>(handle);
    if (!doc) return env->NewStringUTF("");
    return env->NewStringUTF(doc->Html().c_str());
}

static jstring Document_text(JNIEnv* env, jobject, jlong handle) {
    auto* doc = reinterpret_cast<Document*>(handle);
    if (!doc) return env->NewStringUTF("");
    return env->NewStringUTF(doc->Text().c_str());
}

static jlongArray Document_select(JNIEnv* env, jobject, jlong handle, jstring jcss) {
    auto* doc = reinterpret_cast<Document*>(handle);
    if (!doc) return env->NewLongArray(0);
    std::string css = ToStdString(env, jcss);
    
    Selector selector;
    Element root = doc->Body();
    if (!root.GetNativeNode()) {
        root = Element(lxb_dom_interface_node(doc->GetNativeDocument()));
    }
    std::vector<Element> elements = selector.Select(root, css);

    jlongArray arr = env->NewLongArray(elements.size());
    if (elements.empty()) return arr;

    std::vector<jlong> handles;
    handles.reserve(elements.size());
    for (const auto& el : elements) {
        handles.push_back(reinterpret_cast<jlong>(new Element(el)));
    }
    env->SetLongArrayRegion(arr, 0, handles.size(), handles.data());
    return arr;
}

static jstring Element_tagName(JNIEnv* env, jobject, jlong handle) {
    auto* el = reinterpret_cast<Element*>(handle);
    if (!el) return env->NewStringUTF("");
    return env->NewStringUTF(el->TagName().c_str());
}

static jstring Element_id(JNIEnv* env, jobject, jlong handle) {
    auto* el = reinterpret_cast<Element*>(handle);
    if (!el) return env->NewStringUTF("");
    return env->NewStringUTF(el->Id().c_str());
}

static jstring Element_className(JNIEnv* env, jobject, jlong handle) {
    auto* el = reinterpret_cast<Element*>(handle);
    if (!el) return env->NewStringUTF("");
    return env->NewStringUTF(el->ClassName().c_str());
}

static jstring Element_text(JNIEnv* env, jobject, jlong handle) {
    auto* el = reinterpret_cast<Element*>(handle);
    if (!el) return env->NewStringUTF("");
    return env->NewStringUTF(el->Text().c_str());
}

static jstring Element_html(JNIEnv* env, jobject, jlong handle) {
    auto* el = reinterpret_cast<Element*>(handle);
    if (!el) return env->NewStringUTF("");
    return env->NewStringUTF(el->Html().c_str());
}

static jstring Element_attr(JNIEnv* env, jobject, jlong handle, jstring jkey) {
    auto* el = reinterpret_cast<Element*>(handle);
    if (!el) return env->NewStringUTF("");
    std::string key = ToStdString(env, jkey);
    return env->NewStringUTF(el->Attr(key).c_str());
}

static jlongArray Element_select(JNIEnv* env, jobject, jlong handle, jstring jcss) {
    auto* el = reinterpret_cast<Element*>(handle);
    if (!el) return env->NewLongArray(0);
    std::string css = ToStdString(env, jcss);
    
    Selector selector;
    std::vector<Element> elements = selector.Select(*el, css);

    jlongArray arr = env->NewLongArray(elements.size());
    if (elements.empty()) return arr;

    std::vector<jlong> handles;
    handles.reserve(elements.size());
    for (const auto& item : elements) {
        handles.push_back(reinterpret_cast<jlong>(new Element(item)));
    }
    env->SetLongArrayRegion(arr, 0, handles.size(), handles.data());
    return arr;
}

static const JNINativeMethod g_document_methods[] = {
    {"nativeCreate", "()J", reinterpret_cast<void*>(Document_create)},
    {"nativeDestroy", "(J)V", reinterpret_cast<void*>(Document_destroy)},
    {"nativeParse", "(JLjava/lang/String;)Z", reinterpret_cast<void*>(Document_parse)},
    {"nativeTitle", "(J)Ljava/lang/String;", reinterpret_cast<void*>(Document_title)},
    {"nativeHead", "(J)J", reinterpret_cast<void*>(Document_head)},
    {"nativeBody", "(J)J", reinterpret_cast<void*>(Document_body)},
    {"nativeHtml", "(J)Ljava/lang/String;", reinterpret_cast<void*>(Document_html)},
    {"nativeText", "(J)Ljava/lang/String;", reinterpret_cast<void*>(Document_text)},
    {"nativeSelect", "(JLjava/lang/String;)[J", reinterpret_cast<void*>(Document_select)}
};

static const JNINativeMethod g_element_methods[] = {
    {"nativeTagName", "(J)Ljava/lang/String;", reinterpret_cast<void*>(Element_tagName)},
    {"nativeId", "(J)Ljava/lang/String;", reinterpret_cast<void*>(Element_id)},
    {"nativeClassName", "(J)Ljava/lang/String;", reinterpret_cast<void*>(Element_className)},
    {"nativeText", "(J)Ljava/lang/String;", reinterpret_cast<void*>(Element_text)},
    {"nativeHtml", "(J)Ljava/lang/String;", reinterpret_cast<void*>(Element_html)},
    {"nativeAttr", "(JLjava/lang/String;)Ljava/lang/String;", reinterpret_cast<void*>(Element_attr)},
    {"nativeSelect", "(JLjava/lang/String;)[J", reinterpret_cast<void*>(Element_select)}
};

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }

    jclass docClass = env->FindClass("io/github/novelua/lexsoup/Document");
    if (docClass && env->RegisterNatives(docClass, g_document_methods, sizeof(g_document_methods) / sizeof(g_document_methods[0])) < 0) {
        return JNI_ERR;
    }

    jclass elClass = env->FindClass("io/github/novelua/lexsoup/Element");
    if (elClass && env->RegisterNatives(elClass, g_element_methods, sizeof(g_element_methods) / sizeof(g_element_methods[0])) < 0) {
        return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}

} // extern "C"
