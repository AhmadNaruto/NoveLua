#include "novelua/common/jstring_utils.hpp"
#include <utility>

namespace novelua::common {

JString::JString(JNIEnv* env, jstring str)
    : m_env(env), m_jstr(str), m_chars(nullptr) {
    if (m_jstr != nullptr) {
        m_chars = m_env->GetStringUTFChars(m_jstr, nullptr);
    }
}

JString::~JString() {
    if (m_chars != nullptr && m_jstr != nullptr) {
        m_env->ReleaseStringUTFChars(m_jstr, m_chars);
    }
}

JString::JString(JString&& other) noexcept
    : m_env(other.m_env), m_jstr(other.m_jstr), m_chars(other.m_chars) {
    other.m_jstr = nullptr;
    other.m_chars = nullptr;
}

JString& JString::operator=(JString&& other) noexcept {
    if (this != &other) {
        if (m_chars != nullptr && m_jstr != nullptr) {
            m_env->ReleaseStringUTFChars(m_jstr, m_chars);
        }
        m_env = other.m_env;
        m_jstr = other.m_jstr;
        m_chars = other.m_chars;
        other.m_jstr = nullptr;
        other.m_chars = nullptr;
    }
    return *this;
}

const char* JString::Get() const noexcept {
    return m_chars;
}

std::string_view JString::GetView() const noexcept {
    if (m_chars == nullptr) {
        return {};
    }
    return std::string_view(m_chars);
}

std::string JString::ToString() const {
    if (m_chars == nullptr) {
        return "";
    }
    return std::string(m_chars);
}

jstring ToJString(JNIEnv* env, std::string_view str) {
    if (str.empty()) {
        return env->NewStringUTF("");
    }
    // NewStringUTF expects a null-terminated string, so we need to ensure it.
    std::string temp(str);
    return env->NewStringUTF(temp.c_str());
}

std::string ToStdString(JNIEnv* env, jstring str) {
    JString jstr(env, str);
    return jstr.ToString();
}

} // namespace novelua::common
