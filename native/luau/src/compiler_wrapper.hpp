
#pragma once
#include <string>
#include <vector>

namespace novelua::luau {
    class Compiler {
    public:
        static std::string compile(const std::string& source);
    };
}
