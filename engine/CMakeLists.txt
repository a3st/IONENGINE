cmake_minimum_required(VERSION 3.25.1)

add_library(engine STATIC
    ../shadersys/fx.cpp
    ../mdl/mdl.cpp

    graphics/upload_manager.cpp
    graphics/render_queue.cpp
    graphics/camera.cpp
    graphics/buffer.cpp
    graphics/texture.cpp
    graphics/render_pass.cpp
    graphics/shader.cpp
    graphics/graphics.cpp
    window/window.cpp
    engine.cpp)

target_include_directories(engine PUBLIC 
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/engine)

target_precompile_headers(engine PUBLIC ${PROJECT_SOURCE_DIR}/precompiled.h)

target_link_libraries(engine PUBLIC 
    core
    platform
    rhi)

set_target_properties(engine PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})