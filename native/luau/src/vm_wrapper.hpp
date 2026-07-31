#pragma once
#include <lua.h>
#include <lualib.h>
#include <string>

namespace novelua::luau {
    class VM {
    public:
        VM();
        ~VM();
        void close();
        void load(const std::string& bytecode);
        void execute();
        std::string eval(const std::string& script);
        void setGlobal(const std::string& name, int value);
        int getGlobal(const std::string& name);
        lua_State* getState() const { return L; }
    private:
        lua_State* L;
    };
}
