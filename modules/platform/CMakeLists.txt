cmake_minimum_required(VERSION 3.0.0)

add_library(Platform2
    window_loop.cpp
    window_win32.cpp
)

target_include_directories(Platform2 PRIVATE
    ${PROJECT_SOURCE_DIR}/modules
    ${PROJECT_SOURCE_DIR}/thirdparty/range-v3/include
)

target_precompile_headers(Platform2 PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)