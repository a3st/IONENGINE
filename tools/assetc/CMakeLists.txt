cmake_minimum_required(VERSION 3.25.1)

find_package(argh CONFIG REQUIRED)

add_executable(assetc main.cpp)

target_include_directories(assetc PRIVATE 
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/tools/assetc)

target_precompile_headers(assetc PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

target_link_libraries(assetc PRIVATE
    argh
    mdl
    txe)

set_target_properties(assetc PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tools
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tools
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tools)