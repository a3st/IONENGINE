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

    engine/framework.cpp

    # UI
    ui/user_interface.cpp
    ui/system_interface.cpp
    ui/render_interface.cpp

    # Engine
    engine/main.cpp
# ---------------------------
    # Lib
    lib/thread_pool.cpp
    lib/math/color.cpp
    
    lib/scope_profiler.cpp
    lib/logger.cpp
    lib/algorithm.cpp
# ---------------------------
    # Platform
    platform/window_win32.cpp
    platform/window_loop.cpp
# ---------------------------
    # Input
    input/input_manager.cpp
# ---------------------------
    # Asset
    asset/asset_manager.cpp
    asset/mesh.cpp
    asset/texture.cpp
    asset/material.cpp
    asset/technique.cpp
    asset/subscene.cpp
# ---------------------------
    # Scene
    scene/scene_node.cpp
    scene/scene_graph.cpp
    scene/transform_node.cpp
    scene/camera_node.cpp
    scene/mesh_node.cpp
    scene/scene.cpp
    scene/point_light_node.cpp
    scene/streaming_scene.cpp
# ---------------------------    
    # Renderer Backend
    renderer/backend/d3d12x.cpp
    renderer/backend/backend_d3d12.cpp

    # Renderer
    renderer/renderer.cpp
    renderer/ui_renderer.cpp
    renderer/mesh_renderer.cpp
    renderer/shader_binder.cpp
    renderer/shader_program.cpp
    renderer/shader_cache.cpp
    renderer/geometry_buffer.cpp
    renderer/geometry_cache.cpp
    renderer/rt_texture_cache.cpp
    renderer/pipeline_cache.cpp
    renderer/gpu_texture.cpp
    renderer/texture_cache.cpp
    renderer/gpu_buffer.cpp
    renderer/render_queue.cpp
    renderer/upload_manager.cpp
    renderer/frame_graph.cpp
# ---------------------------
    thirdparty/tinyobjloader/src/tiny_obj_loader.cc
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
    thirdparty/xxhash/include
    thirdparty/tinyobjloader/include
    thirdparty/dds/include
    thirdparty/rmlui/include
)

# Link Libs
target_link_libraries(Runtime-Win32
    d3d12.lib
    dxgi.lib
    dxguid.lib
    D3D12MA.lib
    dxcompiler.lib
    freetype.lib
    RmlCore.lib
    RmlDebugger.lib
)

# Link Dirs
target_link_directories(Runtime-Win32 PRIVATE
    thirdparty/d3d12_ma/build/${CMAKE_BUILD_TYPE}
    thirdparty/rmlui/build/${CMAKE_BUILD_TYPE}
    thirdparty/freetype/build/${CMAKE_BUILD_TYPE}
)

# Preprocessor
target_compile_definitions(Runtime-Win32 PRIVATE 
    IONENGINE_PLATFORM_WIN32
    IONENGINE_RENDERER_BACKEND_D3D12
    RMLUI_STATIC_LIB
)

#[[
    Editor-Win32 Build Target
#]]
add_executable(Editor-Win32

    # UI
    ui/user_interface.cpp
    ui/system_interface.cpp
    ui/render_interface.cpp

    # Engine
    editor/main.cpp
    editor/framework.cpp
# ---------------------------
    # Lib
    lib/thread_pool.cpp
    lib/math/color.cpp
    
    lib/scope_profiler.cpp
    lib/logger.cpp
    lib/algorithm.cpp
# ---------------------------
    # Platform
    platform/window_win32.cpp
    platform/window_loop.cpp
# ---------------------------
    # Input
    input/input_manager.cpp
# ---------------------------
    # Asset
    asset/asset_manager.cpp
    asset/mesh.cpp
    asset/texture.cpp
    asset/material.cpp
    asset/technique.cpp
    asset/subscene.cpp
# ---------------------------
    # Scene
    scene/scene_node.cpp
    scene/scene_graph.cpp
    scene/transform_node.cpp
    scene/camera_node.cpp
    scene/mesh_node.cpp
    scene/scene.cpp
    scene/point_light_node.cpp
    scene/streaming_scene.cpp
# ---------------------------    
    # Renderer Backend
    renderer/backend/d3d12x.cpp
    renderer/backend/backend_d3d12.cpp

    # Renderer
    renderer/renderer.cpp
    renderer/ui_renderer.cpp
    renderer/mesh_renderer.cpp
    renderer/shader_binder.cpp
    renderer/shader_program.cpp
    renderer/rt_texture_cache.cpp
    renderer/shader_cache.cpp
    renderer/geometry_buffer.cpp
    renderer/geometry_cache.cpp
    renderer/pipeline_cache.cpp
    renderer/gpu_texture.cpp
    renderer/texture_cache.cpp
    renderer/gpu_buffer.cpp
    renderer/render_queue.cpp
    renderer/upload_manager.cpp
    renderer/frame_graph.cpp
# ---------------------------
    thirdparty/tinyobjloader/src/tiny_obj_loader.cc
)

target_precompile_headers(Editor-Win32 PRIVATE precompiled.h)

# Build Output
set_target_properties(Editor-Win32 PROPERTIES 
    OUTPUT_NAME "editor"
    COMPILE_FLAGS "${COMPILE_FLAGS}"
)

# Include Dirs
target_include_directories(Editor-Win32 PRIVATE
    thirdparty/d3d12_ma/include
    thirdparty/json5/include
    thirdparty/xxhash/include
    thirdparty/tinyobjloader/include
    thirdparty/dds/include
    thirdparty/rmlui/include
)

# Link Libs
target_link_libraries(Editor-Win32
    d3d12.lib
    dxgi.lib
    dxguid.lib
    D3D12MA.lib
    dxcompiler.lib
    freetype.lib
    RmlCore.lib
    RmlDebugger.lib
)

# Link Dirs
target_link_directories(Editor-Win32 PRIVATE
    thirdparty/d3d12_ma/build/${CMAKE_BUILD_TYPE}
    thirdparty/rmlui/build/${CMAKE_BUILD_TYPE}
    thirdparty/freetype/build/${CMAKE_BUILD_TYPE}
)

# Preprocessor
target_compile_definitions(Editor-Win32 PRIVATE 
    IONENGINE_PLATFORM_WIN32
    IONENGINE_RENDERER_BACKEND_D3D12
    RMLUI_STATIC_LIB
)
