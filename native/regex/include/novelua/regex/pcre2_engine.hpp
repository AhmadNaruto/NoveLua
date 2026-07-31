#pragma once

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <optional>

namespace novelua::regex {

struct MatchGroup {
    int index;
    std::string value;
    int start;
    int end;
};

struct MatchResult {
    bool matched;
    int start;
    int end;
    std::vector<MatchGroup> groups;
};

class Pcre2Engine {
public:
    Pcre2Engine(std::string_view pattern, uint32_t options);
    ~Pcre2Engine();

    // Delete copy and move
    Pcre2Engine(const Pcre2Engine&) = delete;
    Pcre2Engine& operator=(const Pcre2Engine&) = delete;
    Pcre2Engine(Pcre2Engine&&) = delete;
    Pcre2Engine& operator=(Pcre2Engine&&) = delete;

    MatchResult match(std::string_view subject, size_t offset = 0);
    std::string replaceFirst(std::string_view subject, std::string_view replacement);
    std::string replaceAll(std::string_view subject, std::string_view replacement);

private:
    pcre2_code* m_code;
    pcre2_match_data* m_match_data;
};

} // namespace novelua::regex
