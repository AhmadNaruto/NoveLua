#pragma once
#include <lua.h>
#include <lualib.h>
#include <string>
#include <vector>

namespace novelua::luau {
    class Table {
    public:
        Table(lua_State* L, int ref);
        ~Table();
        lua_State* getState() const { return L; }
        int getRef() const { return ref; }
        void push() const;
    private:
        lua_State* L;
        int ref;
    };
}
