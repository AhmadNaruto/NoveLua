cmake_minimum_required(VERSION 3.15)
project(novelua C CXX)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if(NOT ANDROID)
    find_package(JNI REQUIRED)
endif()

set(LUAU_EXTERN_C ON CACHE BOOL "Force Luau to compile in C mode" FORCE)
set(LUAU_BUILD_SHARED OFF CACHE BOOL "Force Luau to build static libraries" FORCE)
set(LUAU_BUILD_CLI OFF CACHE BOOL "Disable Luau CLI" FORCE)
set(LUAU_BUILD_TESTS OFF CACHE BOOL "Disable Luau tests" FORCE)

# Add Luau submodule subdirectory
add_subdirectory(src/main/cpp/luau)

set(LUAU_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/luau)

# Create JNI shared library target
add_library(novelua SHARED
    src/main/cpp/luaujava.cpp
    src/main/cpp/luaujavac.cpp
    src/main/cpp/novelua_jni.cpp
)

# Link against Luau static libraries
target_link_libraries(novelua PRIVATE
    Luau.VM
    Luau.Compiler
    Luau.CodeGen
    Luau.Common
    Luau.Ast
    Luau.Bytecode
    Luau.Inliner
)

target_include_directories(novelua PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp
)
if(NOT ANDROID)
    target_include_directories(novelua PRIVATE ${JNI_INCLUDE_DIRS})
endif()

# Export macros
if (MSVC)
    target_compile_definitions(novelua PRIVATE
            "LUACODE_API=extern \"C\" __declspec(dllexport)"
            "LUACODEGEN_API=extern \"C\" __declspec(dllexport)"
            "LUAJITINLINER_API=extern \"C\" __declspec(dllexport)")
else ()
    target_compile_definitions(novelua PRIVATE
            "LUACODE_API=extern \"C\" __attribute__((visibility(\"default\")))"
            "LUACODEGEN_API=extern \"C\" __attribute__((visibility(\"default\")))"
            "LUAJITINLINER_API=extern \"C\" __attribute__((visibility(\"default\")))")
endif ()

# Compile and Link flags for small binary size
if (NOT MSVC)
    target_compile_options(novelua PRIVATE -Os -flto -fvisibility=hidden -ffunction-sections -fdata-sections)
    target_link_options(novelua PRIVATE -flto -Wl,--gc-sections -Wl,--strip-all)
endif ()

# Exclude other unused targets
foreach (unused Luau.CLI.lib Luau.Config Luau.Analysis Luau.Require isocline)
    if(TARGET ${unused})
        set_target_properties(${unused} PROPERTIES EXCLUDE_FROM_ALL TRUE)
    endif()
endforeach ()

