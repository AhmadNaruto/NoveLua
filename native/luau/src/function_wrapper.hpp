#pragma once
#include <lua.h>
#include <lualib.h>

namespace novelua::luau {
    class Function {
    public:
        Function(lua_State* L, int ref);
        ~Function();
        lua_State* getState() const { return L; }
        int getRef() const { return ref; }
        void push() const;
    private:
        lua_State* L;
        int ref;
    };
}
