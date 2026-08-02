#include "function_wrapper.hpp"

namespace novelua::luau {
    Function::Function(lua_State* state, int reference) : L(state), ref(reference) {}
    Function::~Function() {
        if (L && ref != -2) { // -2 is LUA_NOREF
            lua_unref(L, ref);
        }
    }
    void Function::push() const {
        lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
    }
}
