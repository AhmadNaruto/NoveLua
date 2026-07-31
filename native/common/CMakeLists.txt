cmake_minimum_required(VERSION 3.22.1)

project(novelua_common)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/common.cmake)

add_library(novelua_common STATIC
    src/jni_env.cpp
    src/jstring_utils.cpp
    src/exceptions.cpp
)

novelua_apply_target_optimizations(novelua_common)

target_include_directories(novelua_common
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(novelua_common
    PRIVATE
        log
)
