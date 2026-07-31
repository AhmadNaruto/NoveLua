#include "table_wrapper.hpp"
#include <lua.h>
#include <lualib.h>

namespace novelua::luau {
    Table::Table(lua_State* state, int reference) : L(state), ref(reference) {}
    Table::~Table() {
        if (L && ref != 0) {
            lua_unref(L, ref);
        }
    }
    void Table::clear() {
        // stub
    }
    bool Table::contains(const std::string& key) {
        return false; // stub
    }
}
