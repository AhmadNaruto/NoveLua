cmake_minimum_required(VERSION 3.22.1)

project(novelua_lexsoup)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/common.cmake)

set(LEXBOR_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(LEXBOR_BUILD_STATIC ON CACHE BOOL "" FORCE)
set(LEXBOR_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(LEXBOR_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Strip -Werror flags from CMAKE_C_FLAGS and CMAKE_CXX_FLAGS to prevent try_compile check_function_exists failures in CI
string(REGEX REPLACE "-Werror=[a-zA-Z0-9_-]+" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
string(REGEX REPLACE "-Werror=[a-zA-Z0-9_-]+" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
string(REGEX REPLACE "-Werror" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
string(REGEX REPLACE "-Werror" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" CACHE STRING "" FORCE)

set(test_result TRUE CACHE BOOL "" FORCE)

add_subdirectory(../../thirdparty/lexbor lexbor_build EXCLUDE_FROM_ALL)
target_compile_options(lexbor_static PRIVATE -Wno-unused-parameter -Wno-unused-variable -Wno-error)

add_subdirectory(../common common_build EXCLUDE_FROM_ALL)

add_library(novelua_lexsoup SHARED
    src/lexsoup_jni.cpp
    src/document.cpp
    src/element.cpp
    src/selector.cpp
)

novelua_apply_target_optimizations(novelua_lexsoup)
target_compile_options(novelua_lexsoup PRIVATE -Wno-unused-parameter)

target_include_directories(novelua_lexsoup
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ../../thirdparty/lexbor/source
)

target_link_libraries(novelua_lexsoup
    PRIVATE
        novelua_common
        lexbor_static
        log
        m
)
