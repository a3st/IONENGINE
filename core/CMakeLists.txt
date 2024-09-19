cmake_minimum_required(VERSION 3.25.1)

find_package(simdjson CONFIG REQUIRED)

add_library(core INTERFACE)

target_include_directories(core INTERFACE ${PROJECT_SOURCE_DIR})

target_link_libraries(core INTERFACE simdjson)

target_precompile_headers(core INTERFACE ${PROJECT_SOURCE_DIR}/precompiled.h)