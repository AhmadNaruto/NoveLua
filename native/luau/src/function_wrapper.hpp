
#pragma once
#include <lua.h>

namespace novelua::luau {
    class Function {
    public:
        Function(lua_State* L, int ref);
        ~Function();
        void call();
    private:
        [[maybe_unused]] lua_State* L;
        [[maybe_unused]] int ref;
    };
}
