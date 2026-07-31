#include "novelua/regex/pcre2_engine.hpp"
#include <stdexcept>

namespace novelua::regex {

Pcre2Engine::Pcre2Engine(std::string_view pattern, uint32_t options) : m_code(nullptr), m_match_data(nullptr) {
    int errorcode;
    PCRE2_SIZE erroroffset;
    m_code = pcre2_compile(
        reinterpret_cast<PCRE2_SPTR>(pattern.data()),
        pattern.size(),
        options,
        &errorcode,
        &erroroffset,
        nullptr
    );

    if (!m_code) {
        PCRE2_UCHAR buffer[256];
        pcre2_get_error_message(errorcode, buffer, sizeof(buffer));
        throw std::invalid_argument(std::string("PCRE2 compilation failed: ") + reinterpret_cast<char*>(buffer));
    }

    m_match_data = pcre2_match_data_create_from_pattern(m_code, nullptr);
}

Pcre2Engine::~Pcre2Engine() {
    if (m_match_data) {
        pcre2_match_data_free(m_match_data);
    }
    if (m_code) {
        pcre2_code_free(m_code);
    }
}

MatchResult Pcre2Engine::match(std::string_view subject, size_t offset) {
    MatchResult result;
    result.matched = false;

    int rc = pcre2_match(
        m_code,
        reinterpret_cast<PCRE2_SPTR>(subject.data()),
        subject.size(),
        offset,
        0,
        m_match_data,
        nullptr
    );

    if (rc < 0) {
        return result; // No match or error
    }

    result.matched = true;
    PCRE2_SIZE* ovector = pcre2_get_ovector_pointer(m_match_data);
    result.start = static_cast<int>(ovector[0]);
    result.end = static_cast<int>(ovector[1]);

    for (int i = 0; i < rc; ++i) {
        if (ovector[2 * i] != PCRE2_UNSET) {
            MatchGroup group;
            group.index = i;
            group.start = static_cast<int>(ovector[2 * i]);
            group.end = static_cast<int>(ovector[2 * i + 1]);
            group.value = std::string(subject.substr(group.start, group.end - group.start));
            result.groups.push_back(group);
        }
    }

    return result;
}

std::string Pcre2Engine::replaceFirst(std::string_view subject, std::string_view replacement) {
    PCRE2_SIZE outlen = subject.size() + replacement.size() + 128;
    std::string outbuffer(outlen, '\0');

    int rc = pcre2_substitute(
        m_code,
        reinterpret_cast<PCRE2_SPTR>(subject.data()),
        subject.size(),
        0,
        0,
        m_match_data,
        nullptr,
        reinterpret_cast<PCRE2_SPTR>(replacement.data()),
        replacement.size(),
        reinterpret_cast<PCRE2_UCHAR*>(outbuffer.data()),
        &outlen
    );

    if (rc < 0 && rc != PCRE2_ERROR_NOMATCH) {
        throw std::runtime_error("PCRE2 substitution failed");
    }

    outbuffer.resize(outlen);
    return outbuffer;
}

std::string Pcre2Engine::replaceAll(std::string_view subject, std::string_view replacement) {
    PCRE2_SIZE outlen = subject.size() + replacement.size() * 10 + 128;
    std::string outbuffer(outlen, '\0');

    int rc = pcre2_substitute(
        m_code,
        reinterpret_cast<PCRE2_SPTR>(subject.data()),
        subject.size(),
        0,
        PCRE2_SUBSTITUTE_GLOBAL,
        m_match_data,
        nullptr,
        reinterpret_cast<PCRE2_SPTR>(replacement.data()),
        replacement.size(),
        reinterpret_cast<PCRE2_UCHAR*>(outbuffer.data()),
        &outlen
    );

    if (rc < 0 && rc != PCRE2_ERROR_NOMATCH) {
        throw std::runtime_error("PCRE2 substitution failed");
    }

    outbuffer.resize(outlen);
    return outbuffer;
}

} // namespace novelua::regex
