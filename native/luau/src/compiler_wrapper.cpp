
#include "compiler_wrapper.hpp"
#include <luacode.h>
#include <stdexcept>

namespace novelua::luau {
    std::string Compiler::compile(const std::string& source) {
        size_t size = 0;
        char* bytecode = luau_compile(source.c_str(), source.size(), nullptr, &size);
        if (!bytecode) {
            throw std::runtime_error("Luau compile error");
        }
        std::string result(bytecode, size);
        free(bytecode);
        return result;
    }
}
