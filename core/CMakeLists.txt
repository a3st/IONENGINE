cmake_minimum_required(VERSION 3.25.1)

find_package(simdjson CONFIG REQUIRED)

add_library(core STATIC 
    string_utils.cpp
    base64.cpp
    crc32.cpp
    color.cpp)

target_include_directories(core PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(core PUBLIC simdjson::simdjson)

target_precompile_headers(core PUBLIC ${PROJECT_SOURCE_DIR}/precompiled.h)