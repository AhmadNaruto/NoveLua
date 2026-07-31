
#pragma once
#include <lua.h>

namespace novelua::luau {
    class Userdata {
    public:
        Userdata(lua_State* L, void* data);
        ~Userdata();
    private:
        [[maybe_unused]] lua_State* L;
        [[maybe_unused]] void* data;
    };
}
