
#include "thread_wrapper.hpp"

namespace novelua::luau {
    Thread::Thread(lua_State* L) : parentL(L) {
        this->L = lua_newthread(L);
    }
    Thread::~Thread() {}
    void Thread::resume() {}
    int Thread::status() { return lua_status(L); }
    void Thread::close() {}
}
