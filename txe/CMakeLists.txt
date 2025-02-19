cmake_minimum_required(VERSION 3.25.1)

find_package(PNG REQUIRED)

add_library(txe STATIC
    png/png.cpp
    txe.cpp)

target_include_directories(txe PUBLIC 
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/txe)

target_link_libraries(txe PUBLIC 
    core
    PNG::PNG)

target_precompile_headers(txe PUBLIC ${PROJECT_SOURCE_DIR}/precompiled.h)