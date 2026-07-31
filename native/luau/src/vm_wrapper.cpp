#include "vm_wrapper.hpp"
#include <luacode.h>
#include <cstdlib>
#include <stdexcept>

namespace novelua::luau {
    VM::VM() {
        L = luaL_newstate();
        luaL_openlibs(L);
    }
    VM::~VM() {
        close();
    }
    void VM::close() {
        if (L) {
            lua_close(L);
            L = nullptr;
        }
    }
    void VM::load(const std::string& bytecode) {
        if (luau_load(L, "chunk", bytecode.data(), bytecode.size(), 0) != 0) {
            std::string err = lua_tostring(L, -1);
            lua_pop(L, 1);
            throw std::runtime_error("Luau load error: " + err);
        }
    }
    void VM::execute() {
        if (lua_pcall(L, 0, 0, 0) != 0) {
            std::string err = lua_tostring(L, -1);
            lua_pop(L, 1);
            throw std::runtime_error("Luau execute error: " + err);
        }
    }
    std::string VM::eval(const std::string& script) {
        if (!L) return "";
        size_t outSize = 0;
        char* bytecode = luau_compile(script.c_str(), script.length(), nullptr, &outSize);
        if (!bytecode) return "";
        
        if (luau_load(L, "=eval", bytecode, outSize, 0) != 0) {
            std::string err = lua_tostring(L, -1);
            lua_pop(L, 1);
            free(bytecode);
            throw std::runtime_error("Luau compile/load error: " + err);
        }
        free(bytecode);

        if (lua_pcall(L, 0, 1, 0) != 0) {
            std::string err = lua_tostring(L, -1);
            lua_pop(L, 1);
            throw std::runtime_error("Luau eval error: " + err);
        }

        std::string res;
        if (lua_isstring(L, -1)) {
            res = lua_tostring(L, -1);
        } else if (lua_isboolean(L, -1)) {
            res = lua_toboolean(L, -1) ? "true" : "false";
        } else if (lua_isnumber(L, -1)) {
            res = std::to_string(lua_tonumber(L, -1));
        } else if (lua_isnil(L, -1)) {
            res = "nil";
        } else {
            res = "[object]";
        }
        lua_pop(L, 1);
        return res;
    }
    void VM::setGlobal(const std::string& name, int value) {
        lua_pushinteger(L, value);
        lua_setglobal(L, name.c_str());
    }
    int VM::getGlobal(const std::string& name) {
        lua_getglobal(L, name.c_str());
        int val = lua_tointeger(L, -1);
        lua_pop(L, 1);
        return val;
    }
}
