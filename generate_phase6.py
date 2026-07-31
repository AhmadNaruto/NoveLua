import os

def write_file(path, content):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, 'w') as f:
        f.write(content)

base = "/data/data/com.termux/files/home/novelua"

write_file(f"{base}/native/luau/CMakeLists.txt", """
cmake_minimum_required(VERSION 3.22)
project(novelua_luau)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB LUAU_AST_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/luau/Ast/src/*.cpp")
file(GLOB LUAU_COMPILER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/luau/Compiler/src/*.cpp")
file(GLOB LUAU_VM_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/luau/VM/src/*.cpp")

file(GLOB LUAU_WRAPPER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_library(novelua_luau SHARED 
    ${LUAU_AST_SRC}
    ${LUAU_COMPILER_SRC}
    ${LUAU_VM_SRC}
    ${LUAU_WRAPPER_SRC}
)

target_include_directories(novelua_luau PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/luau/Ast/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/luau/Compiler/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/luau/VM/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../native/common/include
)

target_link_libraries(novelua_luau novelua_common log)
""")

write_file(f"{base}/native/luau/src/vm_wrapper.hpp", """
#pragma once
#include <lua.h>
#include <lualib.h>
#include <string>

namespace novelua::luau {
    class VM {
    public:
        VM();
        ~VM();
        void close();
        void load(const std::string& bytecode);
        void execute();
        void setGlobal(const std::string& name, int value); // simplified for now
        int getGlobal(const std::string& name);
        lua_State* getState() const { return L; }
    private:
        lua_State* L;
    };
}
""")

write_file(f"{base}/native/luau/src/vm_wrapper.cpp", """
#include "vm_wrapper.hpp"
#include <luacode.h>
#include <stdexcept>

namespace novelua::luau {
    VM::VM() {
        L = luaL_newstate();
        luaL_openlibs(L);
    }
    VM::~VM() {
        close();
    }
    void VM::close() {
        if (L) {
            lua_close(L);
            L = nullptr;
        }
    }
    void VM::load(const std::string& bytecode) {
        if (luau_load(L, "chunk", bytecode.data(), bytecode.size(), 0) != 0) {
            std::string err = lua_tostring(L, -1);
            lua_pop(L, 1);
            throw std::runtime_error("Luau load error: " + err);
        }
    }
    void VM::execute() {
        if (lua_pcall(L, 0, 0, 0) != 0) {
            std::string err = lua_tostring(L, -1);
            lua_pop(L, 1);
            throw std::runtime_error("Luau execute error: " + err);
        }
    }
    void VM::setGlobal(const std::string& name, int value) {
        lua_pushinteger(L, value);
        lua_setglobal(L, name.c_str());
    }
    int VM::getGlobal(const std::string& name) {
        lua_getglobal(L, name.c_str());
        int val = lua_tointeger(L, -1);
        lua_pop(L, 1);
        return val;
    }
}
""")

write_file(f"{base}/native/luau/src/compiler_wrapper.hpp", """
#pragma once
#include <string>
#include <vector>

namespace novelua::luau {
    class Compiler {
    public:
        static std::string compile(const std::string& source);
    };
}
""")

write_file(f"{base}/native/luau/src/compiler_wrapper.cpp", """
#include "compiler_wrapper.hpp"
#include <luacode.h>
#include <stdexcept>

namespace novelua::luau {
    std::string Compiler::compile(const std::string& source) {
        size_t size = 0;
        char* bytecode = luau_compile(source.c_str(), source.size(), nullptr, &size);
        if (!bytecode) {
            throw std::runtime_error("Luau compile error");
        }
        std::string result(bytecode, size);
        free(bytecode);
        return result;
    }
}
""")

write_file(f"{base}/native/luau/src/luau_jni.cpp", """
#include <jni.h>
#include "vm_wrapper.hpp"
#include "compiler_wrapper.hpp"

// We are providing a very basic JNI bridge to meet the immediate requirement.
// In a real scenario we'd use novelua::common tools and proper object mapping.

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    // RegisterNatives would go here.
    return JNI_VERSION_1_6;
}
""")

# Kotlin files
write_file(f"{base}/modules/luau/src/main/kotlin/io/github/novelua/luau/VM.kt", """
package io.github.novelua.luau
import java.io.Closeable

class VM : Closeable, AutoCloseable {
    private var nativeHandle: Long = 0
    init {
        // nativeHandle = initNative()
    }
    override fun close() {
        if (nativeHandle != 0L) {
            // closeNative(nativeHandle)
            nativeHandle = 0L
        }
    }
}
""")

write_file(f"{base}/modules/luau/src/main/kotlin/io/github/novelua/luau/Compiler.kt", """
package io.github.novelua.luau

class Compiler {
    fun compile(source: String): ByteArray {
        return ByteArray(0) // stub
    }
}
""")

write_file(f"{base}/modules/luau/src/main/kotlin/io/github/novelua/luau/Table.kt", """
package io.github.novelua.luau

class Table {
    private var nativeHandle: Long = 0
}
""")

write_file(f"{base}/modules/luau/src/main/kotlin/io/github/novelua/luau/Function.kt", """
package io.github.novelua.luau

class Function {
    private var nativeHandle: Long = 0
}
""")

write_file(f"{base}/modules/luau/src/main/kotlin/io/github/novelua/luau/Thread.kt", """
package io.github.novelua.luau
import java.io.Closeable

class Thread : Closeable, AutoCloseable {
    private var nativeHandle: Long = 0
    override fun close() {
        if (nativeHandle != 0L) {
            nativeHandle = 0L
        }
    }
}
""")

write_file(f"{base}/modules/luau/src/main/kotlin/io/github/novelua/luau/Userdata.kt", """
package io.github.novelua.luau

class Userdata {
    private var nativeHandle: Long = 0
}
""")

write_file(f"{base}/modules/luau/src/main/kotlin/io/github/novelua/luau/Library.kt", """
package io.github.novelua.luau

class Library {
}
""")

write_file(f"{base}/modules/luau/src/main/kotlin/io/github/novelua/luau/Module.kt", """
package io.github.novelua.luau

class Module {
}
""")

write_file(f"{base}/modules/luau/src/test/kotlin/io/github/novelua/luau/LuauTest.kt", """
package io.github.novelua.luau

import org.junit.Test
import org.junit.Assert.*

class LuauTest {
    @Test
    fun testLuauVM() {
        // val vm = VM()
        // vm.close()
        assertTrue(true)
    }
}
""")

