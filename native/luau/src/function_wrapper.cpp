
#include "function_wrapper.hpp"

namespace novelua::luau {
    Function::Function(lua_State* L, int ref) : L(L), ref(ref) {}
    Function::~Function() {}
    void Function::call() {}
}
