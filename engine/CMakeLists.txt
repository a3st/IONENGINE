cmake_minimum_required(VERSION 3.25.1)

add_library(engine STATIC
    renderer/model.cpp
    renderer/texture.cpp
    renderer/shader.cpp
    renderer/renderer.cpp)

target_include_directories(engine PUBLIC 
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/engine)

target_precompile_headers(engine PUBLIC ${PROJECT_SOURCE_DIR}/precompiled.h)

target_link_libraries(engine PUBLIC 
    core
    platform 
    shadersys
    mdl
    rhi)

set_target_properties(engine PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})