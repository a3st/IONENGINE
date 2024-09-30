cmake_minimum_required(VERSION 3.25.1)

add_library(engine SHARED
    engine.cpp)

target_include_directories(engine PRIVATE ${PROJECT_SOURCE_DIR})

target_precompile_headers(engine PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

target_link_libraries(engine PUBLIC 
    core
    platform 
    shadersys
    rhi
)

set_target_properties(engine PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/engine
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/engine
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/engine)