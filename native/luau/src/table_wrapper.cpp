#include "table_wrapper.hpp"

namespace novelua::luau {
    Table::Table(lua_State* state, int reference) : L(state), ref(reference) {}
    Table::~Table() {
        if (L && ref != -2) { // -2 is LUA_NOREF
            lua_unref(L, ref);
        }
    }
    void Table::push() const {
        lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
    }
}
