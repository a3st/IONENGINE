cmake_minimum_required(VERSION 3.0.0)

project(IONENGINE VERSION 0.0.5)

# Global sets
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${PROJECT_SOURCE_DIR}/)

#[[
    Runtime-Win32 Build Target
#]]
add_executable(Runtime-Win32

    # Engine
    engine/main.cpp
    engine/asset_manager.cpp
    #engine/sponza.cpp
# ---------------------------
    # Lib
    #lib/thread_pool.cpp
    lib/math/color.cpp
# ---------------------------
    # Platform
    platform/window_win32.cpp
    platform/window_loop.cpp
# ---------------------------
    # Asset
    #asset/obj/obj_document.cpp
    asset/mesh.cpp
    asset/texture.cpp
    asset/buffer.cpp
    asset/material.cpp
    asset/shader.cpp
    asset/technique.cpp
# ---------------------------
    # Scene
    #scene/scene_node.cpp
    #scene/transform_node.cpp
    #scene/scene_tree.cpp
    scene/scene.cpp
    scene/mesh_node.cpp
# ---------------------------    
    # Renderer Backend
    renderer/backend/d3d12x.cpp
    renderer/backend/backend_d3d12.cpp

    # Renderer Frontend
    renderer/frontend/context.cpp
    renderer/frontend/geometry_buffer.cpp
    renderer/frontend/frame_graph.cpp
    renderer/frontend/shader_program.cpp

    # Renderer
    renderer/renderer.cpp
# ---------------------------
)

target_precompile_headers(Runtime-Win32 PRIVATE precompiled.h)

# Build Output
set_target_properties(Runtime-Win32 PROPERTIES 
    OUTPUT_NAME "project"
    COMPILE_FLAGS "${COMPILE_FLAGS}"
)

# Include Dirs
target_include_directories(Runtime-Win32 PRIVATE
    thirdparty/d3d12_ma/include
    thirdparty/json5/include
)

# Link Libs
target_link_libraries(Runtime-Win32
    d3d12.lib
    dxgi.lib
    dxguid.lib
    D3D12MA.lib
    dxcompiler.lib
)

# Link Dirs
target_link_directories(Runtime-Win32 PRIVATE
    thirdparty/d3d12_ma/build/Debug
)

# Preprocessor
target_compile_definitions(Runtime-Win32 PRIVATE 
    IONENGINE_PLATFORM_WIN32
    IONENGINE_RENDERER_BACKEND_D3D12
)

#[[
    HLSL SPIRV Package Build Target
#
add_executable(HLSL_SPIRV_Package hlsl_spirv_package/main.cpp)
target_precompile_headers(HLSL_SPIRV_Package PRIVATE precompiled.h)

# Build Output
set_target_properties(HLSL_SPIRV_Package PROPERTIES
    OUTPUT_NAME "hlsl_spirv_package" 
    COMPILE_FLAGS "${COMPILE_FLAGS}"
)

# Include Dirs
target_include_directories(HLSL_SPIRV_Package PRIVATE
    ${PROJECT_SOURCE_DIR}/thirdparty/tclap/include
    ${PROJECT_SOURCE_DIR}/thirdparty/inipp/include
)

#[[
    Asset Compiler Build Target
#
add_executable(AssetCompiler

    asset_compiler/main.cpp
    asset_compiler/asset_compiler.cpp

    asset_compiler/obj_loader.cpp
    #asset_compiler/dds_loader.cpp
    asset_compiler/hlsv_loader.cpp
)

target_precompile_headers(AssetCompiler PRIVATE precompiled.h)

# Build Output
set_target_properties(AssetCompiler PROPERTIES
    OUTPUT_NAME "asset_compiler" 
    COMPILE_FLAGS "${COMPILE_FLAGS}"
)

# Include Dirs
target_include_directories(AssetCompiler PRIVATE
    ${PROJECT_SOURCE_DIR}/thirdparty/tclap/include
)
#]]
