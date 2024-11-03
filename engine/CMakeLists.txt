cmake_minimum_required(VERSION 3.25.1)

add_library(engine STATIC
    renderer/texture.cpp
    renderer/surface.cpp
    renderer/buffer_pool.cpp
    renderer/camera.cpp
    renderer/material.cpp
    renderer/model.cpp
    renderer/shader.cpp
    renderer/renderer.cpp
    
    scene/entity.cpp
    scene/components/transform2d.cpp
    scene/components/sprite_renderer.cpp)

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