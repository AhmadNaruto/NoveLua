#pragma once

#include <jni.h>
#include <utility>

namespace novelua::common {

/**
 * RAII wrapper for JNI local references.
 * Automatically calls DeleteLocalRef on destruction.
 */
template <typename T>
class ScopedLocalRef {
public:
    ScopedLocalRef(JNIEnv* env, T ref) : m_env(env), m_ref(ref) {}

    ~ScopedLocalRef() {
        if (m_ref != nullptr) {
            m_env->DeleteLocalRef(m_ref);
        }
    }

    ScopedLocalRef(const ScopedLocalRef&) = delete;
    ScopedLocalRef& operator=(const ScopedLocalRef&) = delete;

    ScopedLocalRef(ScopedLocalRef&& other) noexcept
        : m_env(other.m_env), m_ref(other.m_ref) {
        other.m_ref = nullptr;
    }

    ScopedLocalRef& operator=(ScopedLocalRef&& other) noexcept {
        if (this != &other) {
            if (m_ref != nullptr) {
                m_env->DeleteLocalRef(m_ref);
            }
            m_env = other.m_env;
            m_ref = other.m_ref;
            other.m_ref = nullptr;
        }
        return *this;
    }

    T Get() const noexcept { return m_ref; }
    
    // Implicit conversion
    operator T() const noexcept { return m_ref; }

    T Release() noexcept {
        T ref = m_ref;
        m_ref = nullptr;
        return ref;
    }

private:
    JNIEnv* m_env;
    T m_ref;
};

} // namespace novelua::common
