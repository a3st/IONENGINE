cmake_minimum_required(VERSION 3.0.0)

add_library(Math
    src/color.cpp
)

target_precompile_headers(Math PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

target_include_directories(Math PUBLIC
    ${PROJECT_SOURCE_DIR}/math/include
)