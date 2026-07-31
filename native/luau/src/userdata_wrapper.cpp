
#include "userdata_wrapper.hpp"

namespace novelua::luau {
    Userdata::Userdata(lua_State* L, void* data) : L(L), data(data) {}
    Userdata::~Userdata() {}
}
