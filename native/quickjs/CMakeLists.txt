cmake_minimum_required(VERSION 3.22.1)

project(novelua_quickjs)

set(CMAKE_C_EXTENSIONS ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/common.cmake)

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/quickjs/VERSION" QUICKJS_VERSION)
string(STRIP "${QUICKJS_VERSION}" QUICKJS_VERSION)

add_definitions(-DCONFIG_VERSION="${QUICKJS_VERSION}")
add_definitions(-D_GNU_SOURCE)
add_definitions(-DCONFIG_BIGNUM)

add_library(novelua_quickjs SHARED
    ../../thirdparty/quickjs/cutils.c
    ../../thirdparty/quickjs/libregexp.c
    ../../thirdparty/quickjs/libunicode.c
    ../../thirdparty/quickjs/dtoa.c
    ../../thirdparty/quickjs/quickjs.c
    src/runtime_wrapper.cpp
    src/context_wrapper.cpp
    src/value_wrapper.cpp
    src/quickjs_jni.cpp
)

novelua_apply_target_optimizations(novelua_quickjs)
set_target_properties(novelua_quickjs PROPERTIES C_STANDARD 17 C_EXTENSIONS ON)
target_compile_options(novelua_quickjs PRIVATE -w)

target_include_directories(novelua_quickjs
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/quickjs
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/include
)

target_link_libraries(novelua_quickjs
    PRIVATE
        novelua_common
        log
)

add_subdirectory(../common common_lib EXCLUDE_FROM_ALL)
