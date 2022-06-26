cmake_minimum_required(VERSION 3.0.0)

add_library(Input
    input_manager.cpp
)

target_include_directories(Input PRIVATE
    ${PROJECT_SOURCE_DIR}/modules
    ${PROJECT_SOURCE_DIR}/thirdparty/range-v3/include
)

target_precompile_headers(Input PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)