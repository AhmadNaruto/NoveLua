cmake_minimum_required(VERSION 3.22)
project(novelua_regex CXX)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/common.cmake)

set(PCRE2_BUILD_PCRE2_8 ON CACHE BOOL "" FORCE)
set(PCRE2_BUILD_PCRE2_16 OFF CACHE BOOL "" FORCE)
set(PCRE2_BUILD_PCRE2_32 OFF CACHE BOOL "" FORCE)
set(PCRE2_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(PCRE2_BUILD_PCRE2GREP OFF CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/pcre2 thirdparty_pcre2 EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../common common_build EXCLUDE_FROM_ALL)

add_library(novelua_regex SHARED
    src/pcre2_engine.cpp
    src/regex_jni.cpp
)

novelua_apply_target_optimizations(novelua_regex)

target_include_directories(novelua_regex
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/include
)

target_link_libraries(novelua_regex
    PUBLIC
        pcre2-8
        novelua_common
        log
)
