set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT ANDROID_ABI STREQUAL "arm64-v8a")
    message(FATAL_ERROR "Only arm64-v8a ABI is supported")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "MinSizeRel" CACHE STRING "Choose the type of build." FORCE)
endif()

function(novelua_apply_target_optimizations TARGET_NAME)
    if(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
        set(OPT_FLAG "-Oz")
        set(LTO_FLAG "-flto=thin")
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        set(OPT_FLAG "-O3")
        set(LTO_FLAG "-flto=thin")
    else()
        set(OPT_FLAG "-Os")
        set(LTO_FLAG "-flto=thin")
    endif()

    target_compile_options(${TARGET_NAME} PRIVATE
        ${OPT_FLAG}
        ${LTO_FLAG}
        -ffunction-sections
        -fdata-sections
        -fvisibility=hidden
        -fvisibility-inlines-hidden
        -fno-rtti
        -Wall
        -Wextra
    )

    target_link_options(${TARGET_NAME} PRIVATE
        ${LTO_FLAG}
        -Wl,--gc-sections
        -Wl,--strip-all
        -Wl,--icf=all
        -nostdlib++
    )

    if(CMAKE_ANDROID_STL_TYPE STREQUAL "c++_static" OR ANDROID_STL STREQUAL "c++_static")
        target_link_libraries(${TARGET_NAME} PRIVATE c++_static c++abi)
    endif()
endfunction()
