# Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

cmake_minimum_required(VERSION 3.0.0)
project(lgfx VERSION 0.1.0)

include_directories(${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(lgfx-d3d12
    d3d12/d3d12.cpp
    d3d12/device.cpp
    d3d12/fence.cpp
    d3d12/memory_pool.cpp
    d3d12/descriptor_pool.cpp
    d3d12/conversion.cpp
    d3d12/texture.cpp
    d3d12/texture_view.cpp
    d3d12/buffer.cpp
    d3d12/buffer_view.cpp
    d3d12/pipeline.cpp
    d3d12/command_buffer.cpp
    d3d12/frame_buffer.cpp
    d3d12/render_pass.cpp
)

target_precompile_headers(lgfx-d3d12 PRIVATE precompiled.h)

set_target_properties(lgfx-d3d12 PROPERTIES 
    OUTPUT_NAME "lgfx-d3d12"
    COMPILE_FLAGS "${COMPILE_FLAGS}"
)

target_compile_definitions(lgfx-d3d12 PRIVATE 
    LGFX_BACKEND_D3D12
)
