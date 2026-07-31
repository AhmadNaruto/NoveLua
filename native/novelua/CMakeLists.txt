cmake_minimum_required(VERSION 3.22.1)
project(novelua C CXX)

set(CMAKE_C_EXTENSIONS ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/common.cmake)

# Third-party configurations
set(PCRE2_BUILD_PCRE2_8 ON CACHE BOOL "" FORCE)
set(PCRE2_BUILD_PCRE2_16 OFF CACHE BOOL "" FORCE)
set(PCRE2_BUILD_PCRE2_32 OFF CACHE BOOL "" FORCE)
set(PCRE2_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(PCRE2_BUILD_PCRE2GREP OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/pcre2 thirdparty_pcre2 EXCLUDE_FROM_ALL)

set(LEXBOR_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(LEXBOR_BUILD_STATIC ON CACHE BOOL "" FORCE)
set(LEXBOR_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(LEXBOR_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CMAKE_REQUIRED_LIBRARIES m)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/lexbor lexbor_build EXCLUDE_FROM_ALL)
target_compile_options(lexbor_static PRIVATE -Wno-unused-parameter -Wno-unused-variable -Wno-error)

# QuickJS definitions
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/quickjs/VERSION" QUICKJS_VERSION)
string(STRIP "${QUICKJS_VERSION}" QUICKJS_VERSION)
add_definitions(-DCONFIG_VERSION="${QUICKJS_VERSION}")
add_definitions(-D_GNU_SOURCE)
add_definitions(-DCONFIG_BIGNUM)

# Sources
file(GLOB COMMON_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../common/src/*.cpp")
file(GLOB REGEX_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../regex/src/*.cpp")
file(GLOB LEXSOUP_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../lexsoup/src/*.cpp")
file(GLOB QUICKJS_WRAPPER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../quickjs/src/*.cpp")

file(GLOB LUAU_COMMON_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/luau/Common/src/*.cpp")
file(GLOB LUAU_AST_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/luau/Ast/src/*.cpp")
file(GLOB LUAU_BYTECODE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/luau/Bytecode/src/*.cpp")
file(GLOB LUAU_COMPILER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/luau/Compiler/src/*.cpp")
file(GLOB LUAU_VM_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/luau/VM/src/*.cpp")
file(GLOB LUAU_WRAPPER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../luau/src/*.cpp")

add_library(novelua SHARED
    ${COMMON_SRC}
    ${REGEX_SRC}
    ${LEXSOUP_SRC}
    ${QUICKJS_WRAPPER_SRC}
    ../../thirdparty/quickjs/cutils.c
    ../../thirdparty/quickjs/libregexp.c
    ../../thirdparty/quickjs/libunicode.c
    ../../thirdparty/quickjs/dtoa.c
    ../../thirdparty/quickjs/quickjs.c
    ${LUAU_COMMON_SRC}
    ${LUAU_AST_SRC}
    ${LUAU_BYTECODE_SRC}
    ${LUAU_COMPILER_SRC}
    ${LUAU_VM_SRC}
    ${LUAU_WRAPPER_SRC}
    src/novelua_jni.cpp
)

novelua_apply_target_optimizations(novelua)
set_target_properties(novelua PROPERTIES C_STANDARD 17 C_EXTENSIONS ON)
target_compile_options(novelua PRIVATE -Wno-unused-parameter -Wno-unused-variable -Wno-sign-compare -Wno-error)

target_include_directories(novelua PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../regex/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../lexsoup/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../quickjs/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../luau/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/quickjs
    ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/lexbor/source
    ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/luau/Common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/luau/Ast/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/luau/Bytecode/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/luau/Compiler/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/luau/VM/include
)

target_link_libraries(novelua PRIVATE
    pcre2-8
    lexbor_static
    log
    m
)
