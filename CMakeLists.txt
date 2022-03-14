cmake_minimum_required(VERSION 3.0.0)

project(IONENGINE VERSION 0.1.0)

# Global sets
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DNDEBUG")

include_directories(${PROJECT_SOURCE_DIR}/)

#[[
    Runtime-Win32 Build Target
#]]

add_executable(Runtime-Win32

    # Engine
    engine/main.cpp
    engine/asset_manager.cpp

    # Lib
    lib/thread_pool.cpp

    # Platform
    platform/window_win32.cpp
    platform/window_loop_win32.cpp
    
    # API
    renderer/d3d12/memory_allocator.cpp
    renderer/d3d12/descriptor_allocator.cpp

    # Renderer
    renderer/backend_d3d12.cpp
    renderer/world_renderer.cpp
    renderer/frame_graph.cpp
    renderer/color.cpp
    renderer/shader_graph.cpp
    renderer/material_manager.cpp

    # Project
    project/world_controller.cpp
)

target_precompile_headers(Runtime-Win32 PRIVATE precompiled.h)

# Build Output
set_target_properties(Runtime-Win32 PROPERTIES 
    OUTPUT_NAME "project"
    COMPILE_FLAGS "${COMPILE_FLAGS}"
)

# Include Dirs
target_include_directories(Runtime-Win32 PRIVATE

)

# Link Libs
target_link_libraries(Runtime-Win32
    ${Runtime-Win32}
    d3d12.lib
    dxgi.lib
    dxguid.lib
)

# Link Dirs
target_link_directories(Runtime-Win32 PRIVATE

)

# Preprocessor
target_compile_definitions(Runtime-Win32 PRIVATE 
    IONENGINE_PLATFORM_WIN32
    IONENGINE_BACKEND_D3D12
)

#[[
    HLSL SPIRV Package Build Target
#]]

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
)

#[[
    Asset Compiler Build Target
#]]

add_executable(AssetCompiler

    asset_compiler/main.cpp
    asset_compiler/asset_compiler.cpp

    #asset_compiler/obj_loader.cpp
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
