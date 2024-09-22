cmake_minimum_required(VERSION 3.25.1)

add_library(math STATIC color.cpp)

target_include_directories(math PRIVATE ${PROJECT_SOURCE_DIR})

target_precompile_headers(math PUBLIC ${PROJECT_SOURCE_DIR}/precompiled.h)