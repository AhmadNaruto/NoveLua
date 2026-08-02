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

static jstring Node_nodeName(JNIEnv* env, jobject, jlong handle) {
    auto* el = reinterpret_cast<Element*>(handle);
    if (!el || !el->GetNativeNode()) return env->NewStringUTF("");
    size_t len;
    const lxb_char_t* name = lxb_dom_node_name(el->GetNativeNode(), &len);
    if (!name) return env->NewStringUTF("");
    return env->NewStringUTF(std::string((const char*)name, len).c_str());
}

static jstring Node_outerHtml(JNIEnv* env, jobject, jlong handle) {
    auto* el = reinterpret_cast<Element*>(handle);
    if (!el || !el->GetNativeNode()) return env->NewStringUTF("");
    lexbor_str_t str = {};
    lxb_status_t status = lxb_html_serialize_tree_str(el->GetNativeNode(), &str);
    if (status != LXB_STATUS_OK) return env->NewStringUTF("");
    jstring res = env->NewStringUTF((const char*)str.data);
    lexbor_str_destroy(&str, el->GetNativeNode()->owner_document->text, false);
    return res;
}

static jlong Node_parent(JNIEnv*, jobject, jlong handle) {
    auto* el = reinterpret_cast<Element*>(handle);
    if (!el || !el->GetNativeNode() || !el->GetNativeNode()->parent) return 0;
    return reinterpret_cast<jlong>(new Element(el->GetNativeNode()->parent));
}

static jlongArray Node_childNodes(JNIEnv* env, jobject, jlong handle) {
    auto* el = reinterpret_cast<Element*>(handle);
    if (!el || !el->GetNativeNode()) return env->NewLongArray(0);
    std::vector<jlong> handles;
    for (lxb_dom_node_t* child = el->GetNativeNode()->first_child; child; child = child->next) {
        handles.push_back(reinterpret_cast<jlong>(new Element(child)));
    }
    jlongArray arr = env->NewLongArray(handles.size());
    if (!handles.empty()) {
        env->SetLongArrayRegion(arr, 0, handles.size(), handles.data());
    }
    return arr;
}

static void Node_remove(JNIEnv*, jobject, jlong handle) {
    auto* el = reinterpret_cast<Element*>(handle);
    if (el && el->GetNativeNode()) {
        lxb_dom_node_remove(el->GetNativeNode());
    }
}

static jint Node_nodeType(JNIEnv*, jobject, jlong handle) {
    auto* el = reinterpret_cast<Element*>(handle);
    if (!el || !el->GetNativeNode()) return 0;
    return static_cast<jint>(el->GetNativeNode()->type);
}

static void Node_destroy(JNIEnv*, jobject, jlong handle) {
    if (handle != 0) {
        delete reinterpret_cast<Element*>(handle);
    }
}

static jstring TextNode_text(JNIEnv* env, jobject, jlong handle) {
    auto* el = reinterpret_cast<Element*>(handle);
    if (!el || !el->GetNativeNode()) return env->NewStringUTF("");
    size_t len;
    lxb_char_t* text = lxb_dom_node_text_content(el->GetNativeNode(), &len);
    if (!text) return env->NewStringUTF("");
    std::string res((const char*)text, len);
    lexbor_free(text);
    return env->NewStringUTF(res.c_str());
}

static jlongArray Element_children(JNIEnv* env, jobject, jlong handle) {
    auto* el = reinterpret_cast<Element*>(handle);
    if (!el) return env->NewLongArray(0);
    std::vector<Element> children = el->Children();
    jlongArray arr = env->NewLongArray(children.size());
    if (children.empty()) return arr;
    std::vector<jlong> handles;
    handles.reserve(children.size());
    for (const auto& item : children) {
        handles.push_back(reinterpret_cast<jlong>(new Element(item)));
    }
    env->SetLongArrayRegion(arr, 0, handles.size(), handles.data());
    return arr;
}

static jlong Element_child(JNIEnv*, jobject, jlong handle, jint index) {
    auto* el = reinterpret_cast<Element*>(handle);
    if (!el) return 0;
    Element child = el->Child(index);
    if (!child.GetNativeNode()) return 0;
    return reinterpret_cast<jlong>(new Element(child));
}

static jlong Element_parent(JNIEnv*, jobject, jlong handle) {
    auto* el = reinterpret_cast<Element*>(handle);
    if (!el) return 0;
    Element parent = el->Parent();
    if (!parent.GetNativeNode()) return 0;
    return reinterpret_cast<jlong>(new Element(parent));
}

static jlong Element_next(JNIEnv*, jobject, jlong handle) {
    auto* el = reinterpret_cast<Element*>(handle);
    if (!el) return 0;
    Element next = el->Next();
    if (!next.GetNativeNode()) return 0;
    return reinterpret_cast<jlong>(new Element(next));
}

static jlong Element_previous(JNIEnv*, jobject, jlong handle) {
    auto* el = reinterpret_cast<Element*>(handle);
    if (!el) return 0;
    Element prev = el->Previous();
    if (!prev.GetNativeNode()) return 0;
    return reinterpret_cast<jlong>(new Element(prev));
}

static void Element_empty(JNIEnv*, jobject, jlong handle) {
    auto* el = reinterpret_cast<Element*>(handle);
    if (el) el->Empty();
}

static jboolean Element_hasAttr(JNIEnv* env, jobject, jlong handle, jstring jkey) {
    auto* el = reinterpret_cast<Element*>(handle);
    if (!el) return JNI_FALSE;
    std::string key = ToStdString(env, jkey);
    return el->HasAttr(key) ? JNI_TRUE : JNI_FALSE;
}

static void Element_removeAttr(JNIEnv* env, jobject, jlong handle, jstring jkey) {
    auto* el = reinterpret_cast<Element*>(handle);
    if (el) {
        std::string key = ToStdString(env, jkey);
        el->RemoveAttr(key);
    }
}

static jboolean Element_setAttr(JNIEnv* env, jobject, jlong handle, jstring jkey, jstring jval) {
    auto* el = reinterpret_cast<Element*>(handle);
    if (!el) return JNI_FALSE;
    std::string key = ToStdString(env, jkey);
    std::string val = ToStdString(env, jval);
    return el->SetAttr(key, val) ? JNI_TRUE : JNI_FALSE;
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
    {"nativeSelect", "(JLjava/lang/String;)[J", reinterpret_cast<void*>(Element_select)},
    {"nativeChildren", "(J)[J", reinterpret_cast<void*>(Element_children)},
    {"nativeChild", "(JI)J", reinterpret_cast<void*>(Element_child)},
    {"nativeParentElement", "(J)J", reinterpret_cast<void*>(Element_parent)},
    {"nativeNext", "(J)J", reinterpret_cast<void*>(Element_next)},
    {"nativePrevious", "(J)J", reinterpret_cast<void*>(Element_previous)},
    {"nativeEmpty", "(J)V", reinterpret_cast<void*>(Element_empty)},
    {"nativeHasAttr", "(JLjava/lang/String;)Z", reinterpret_cast<void*>(Element_hasAttr)},
    {"nativeRemoveAttr", "(JLjava/lang/String;)V", reinterpret_cast<void*>(Element_removeAttr)},
    {"nativeSetAttr", "(JLjava/lang/String;Ljava/lang/String;)Z", reinterpret_cast<void*>(Element_setAttr)}
};

static const JNINativeMethod g_node_methods[] = {
    {"nativeNodeName", "(J)Ljava/lang/String;", reinterpret_cast<void*>(Node_nodeName)},
    {"nativeOuterHtml", "(J)Ljava/lang/String;", reinterpret_cast<void*>(Node_outerHtml)},
    {"nativeParent", "(J)J", reinterpret_cast<void*>(Node_parent)},
    {"nativeChildNodes", "(J)[J", reinterpret_cast<void*>(Node_childNodes)},
    {"nativeRemove", "(J)V", reinterpret_cast<void*>(Node_remove)},
    {"nativeNodeType", "(J)I", reinterpret_cast<void*>(Node_nodeType)},
    {"nativeDestroy", "(J)V", reinterpret_cast<void*>(Node_destroy)}
};

static const JNINativeMethod g_textnode_methods[] = {
    {"nativeText", "(J)Ljava/lang/String;", reinterpret_cast<void*>(TextNode_text)}
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

    jclass nodeClass = env->FindClass("io/github/novelua/lexsoup/Node");
    if (nodeClass && env->RegisterNatives(nodeClass, g_node_methods, sizeof(g_node_methods) / sizeof(g_node_methods[0])) < 0) {
        return JNI_ERR;
    }

    jclass textClass = env->FindClass("io/github/novelua/lexsoup/TextNode");
    if (textClass && env->RegisterNatives(textClass, g_textnode_methods, sizeof(g_textnode_methods) / sizeof(g_textnode_methods[0])) < 0) {
        return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}

} // extern "C"
