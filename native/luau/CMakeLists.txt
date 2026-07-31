cmake_minimum_required(VERSION 3.22)
project(novelua_luau)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/common.cmake)

file(GLOB LUAU_COMMON_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/luau/Common/src/*.cpp")
file(GLOB LUAU_AST_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/luau/Ast/src/*.cpp")
file(GLOB LUAU_BYTECODE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/luau/Bytecode/src/*.cpp")
file(GLOB LUAU_COMPILER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/luau/Compiler/src/*.cpp")
file(GLOB LUAU_VM_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/luau/VM/src/*.cpp")

file(GLOB LUAU_WRAPPER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_subdirectory(../common common_build EXCLUDE_FROM_ALL)

add_library(novelua_luau SHARED 
    ${LUAU_COMMON_SRC}
    ${LUAU_AST_SRC}
    ${LUAU_BYTECODE_SRC}
    ${LUAU_COMPILER_SRC}
    ${LUAU_VM_SRC}
    ${LUAU_WRAPPER_SRC}
)

novelua_apply_target_optimizations(novelua_luau)
target_compile_options(novelua_luau PRIVATE -Wno-unused-parameter -Wno-unused-variable -Wno-error)

target_include_directories(novelua_luau PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/luau/Common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/luau/Ast/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/luau/Bytecode/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/luau/Compiler/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/luau/VM/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../native/common/include
)

target_link_libraries(novelua_luau PRIVATE novelua_common log)
