
#pragma once
#include <lua.h>

namespace novelua::luau {
    class Thread {
    public:
        Thread(lua_State* L);
        ~Thread();
        void resume();
        int status();
        void close();
    private:
        [[maybe_unused]] lua_State* parentL;
        [[maybe_unused]] lua_State* L;
    };
}
