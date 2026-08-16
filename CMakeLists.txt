cmake_minimum_required(VERSION 3.15)
project(novelua C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if(NOT ANDROID)
    find_package(JNI REQUIRED)
endif()

set(LUAU_EXTERN_C ON CACHE BOOL "Force Luau to compile in C mode" FORCE)
set(LUAU_BUILD_SHARED ON CACHE BOOL "Force Luau to build shared libraries" FORCE)
set(LUAU_BUILD_CLI OFF CACHE BOOL "Disable Luau CLI" FORCE)
set(LUAU_BUILD_TESTS OFF CACHE BOOL "Disable Luau tests" FORCE)

# Add Luau submodule subdirectory
add_subdirectory(src/main/cpp/luau)

set(LUAU_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/luau)

# Add our custom sources to Luau.VM target
target_sources(Luau.VM PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/luaujava.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/luaujavac.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/novelua_jni.cpp
)

target_include_directories(Luau.VM PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp
)
if(NOT ANDROID)
    target_include_directories(Luau.VM PRIVATE ${JNI_INCLUDE_DIRS})
endif()

# Helper function to absorb sources of dependencies into Luau.VM
function(luaujava_absorb target)
    foreach (support ${ARGN})
        get_target_property(dir ${support} SOURCE_DIR)
        get_target_property(srcs ${support} SOURCES)
        foreach (src ${srcs})
            if (NOT IS_ABSOLUTE ${src})
                set(src ${dir}/${src})
            endif ()
            target_sources(${target} PRIVATE ${src})
        endforeach ()
        set_target_properties(${support} PROPERTIES EXCLUDE_FROM_ALL TRUE)
    endforeach ()
endfunction()

luaujava_absorb(Luau.VM
    Luau.Common Luau.Ast Luau.Bytecode Luau.Compiler Luau.CodeGen Luau.Inliner
)

set_target_properties(Luau.VM PROPERTIES LINK_LIBRARIES "" INTERFACE_LINK_LIBRARIES "")
target_include_directories(Luau.VM PRIVATE
    ${LUAU_DIR}/Common/include
    ${LUAU_DIR}/Ast/include
    ${LUAU_DIR}/Bytecode/include
    ${LUAU_DIR}/Bytecode/src
    ${LUAU_DIR}/Compiler/include
    ${LUAU_DIR}/CodeGen/include
    ${LUAU_DIR}/CodeGen/src
    ${LUAU_DIR}/Inliner/include
    ${LUAU_DIR}/VM/src
)

# Export macros
if (MSVC)
    target_compile_definitions(Luau.VM PRIVATE
            "LUACODE_API=extern \"C\" __declspec(dllexport)"
            "LUACODEGEN_API=extern \"C\" __declspec(dllexport)"
            "LUAJITINLINER_API=extern \"C\" __declspec(dllexport)")
else ()
    target_compile_definitions(Luau.VM PRIVATE
            "LUACODE_API=extern \"C\" __attribute__((visibility(\"default\")))"
            "LUACODEGEN_API=extern \"C\" __attribute__((visibility(\"default\")))"
            "LUAJITINLINER_API=extern \"C\" __attribute__((visibility(\"default\")))")
endif ()

# Compile and Link flags for small binary size
if (NOT MSVC)
    target_compile_options(Luau.VM PRIVATE -Os -flto -fvisibility=hidden -ffunction-sections -fdata-sections)
    target_link_options(Luau.VM PRIVATE -flto -Wl,--gc-sections -Wl,--strip-all)
endif ()

# Exclude other unused targets
foreach (unused Luau.CLI.lib Luau.Config Luau.Analysis Luau.Require isocline)
    if(TARGET ${unused})
        set_target_properties(${unused} PROPERTIES EXCLUDE_FROM_ALL TRUE)
    endif()
endforeach ()

# Set final output library name to novelua (produces libnovelua.so)
set_target_properties(Luau.VM PROPERTIES OUTPUT_NAME novelua)
