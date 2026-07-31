
#pragma once
#include <lua.h>
#include <string>
#include <vector>

namespace novelua::luau {
    class Table {
    public:
        Table(lua_State* L, int ref);
        ~Table();
        void clear();
        bool contains(const std::string& key);
    private:
        lua_State* L;
        int ref;
    };
}
