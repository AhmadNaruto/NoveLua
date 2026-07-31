#pragma once

#include <jni.h>
#include <string>
#include <string_view>

namespace novelua::common {

/**
 * RAII wrapper for jstring to manage UTF-8 chars lifetime.
 */
class JString {
public:
    JString(JNIEnv* env, jstring str);
    ~JString();

    JString(const JString&) = delete;
    JString& operator=(const JString&) = delete;

    JString(JString&& other) noexcept;
    JString& operator=(JString&& other) noexcept;

    const char* Get() const noexcept;
    std::string_view GetView() const noexcept;
    std::string ToString() const;

private:
    JNIEnv* m_env;
    jstring m_jstr;
    const char* m_chars;
};

/**
 * Helper to convert std::string_view to jstring.
 */
jstring ToJString(JNIEnv* env, std::string_view str);

/**
 * Helper to convert jstring to std::string.
 */
std::string ToStdString(JNIEnv* env, jstring str);

} // namespace novelua::common
