import os

def write_file(path, content):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, 'w') as f:
        f.write(content)

base = "/data/data/com.termux/files/home/novelua"

write_file(f"{base}/native/luau/src/table_wrapper.hpp", """
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
""")

write_file(f"{base}/native/luau/src/table_wrapper.cpp", """
#include "table_wrapper.hpp"
#include <lualib.h>

namespace novelua::luau {
    Table::Table(lua_State* state, int reference) : L(state), ref(reference) {}
    Table::~Table() {
        luaL_unref(L, LUA_REGISTRYINDEX, ref);
    }
    void Table::clear() {
        // stub
    }
    bool Table::contains(const std::string& key) {
        return false; // stub
    }
}
""")

write_file(f"{base}/native/luau/src/thread_wrapper.hpp", """
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
        lua_State* parentL;
        lua_State* L;
    };
}
""")

write_file(f"{base}/native/luau/src/thread_wrapper.cpp", """
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
""")

write_file(f"{base}/native/luau/src/function_wrapper.hpp", """
#pragma once
#include <lua.h>

namespace novelua::luau {
    class Function {
    public:
        Function(lua_State* L, int ref);
        ~Function();
        void call();
    private:
        lua_State* L;
        int ref;
    };
}
""")

write_file(f"{base}/native/luau/src/function_wrapper.cpp", """
#include "function_wrapper.hpp"

namespace novelua::luau {
    Function::Function(lua_State* L, int ref) : L(L), ref(ref) {}
    Function::~Function() {}
    void Function::call() {}
}
""")

write_file(f"{base}/native/luau/src/userdata_wrapper.hpp", """
#pragma once
#include <lua.h>

namespace novelua::luau {
    class Userdata {
    public:
        Userdata(lua_State* L, void* data);
        ~Userdata();
    private:
        lua_State* L;
        void* data;
    };
}
""")

write_file(f"{base}/native/luau/src/userdata_wrapper.cpp", """
#include "userdata_wrapper.hpp"

namespace novelua::luau {
    Userdata::Userdata(lua_State* L, void* data) : L(L), data(data) {}
    Userdata::~Userdata() {}
}
""")

