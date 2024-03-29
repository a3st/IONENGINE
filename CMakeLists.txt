cmake_minimum_required(VERSION 3.26.0)
project(IONENGINE VERSION 0.0.5)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_subdirectory(thirdparty/simdjson)
add_subdirectory(thirdparty/tinygltf)
add_subdirectory(thirdparty/libpng)
add_subdirectory(thirdparty/libwebview)
add_subdirectory(thirdparty/base64pp)
add_subdirectory(thirdparty/d3d12ma)

if (WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif ()

add_library(platform SHARED
    "platform/windows/window.cpp"
    "platform/window.cpp"
)

target_precompile_headers(platform PRIVATE precompiled.h)

target_include_directories(platform PUBLIC
    "${PROJECT_SOURCE_DIR}"
)

target_link_directories(platform PUBLIC
)

target_link_libraries(platform PUBLIC
)

target_compile_definitions(platform PUBLIC 
    IONENGINE_PLATFORM_WIN32
)

add_library(engine SHARED
    "math/color.cpp"

    "rhi/dx12/rhi.cpp"
    "rhi/rhi.cpp"
    "rhi/shader_file.cpp"

    "engine/material.cpp"
    "engine/model.cpp"
    "engine/texture.cpp"
    "engine/engine.cpp"
    "engine/linked_device.cpp"
    "engine/asset_loader.cpp"
    "engine/renderer.cpp"
    "engine/shader_manager.cpp"

    "extensions/importers/glb.cpp"
)

target_precompile_headers(engine PRIVATE precompiled.h)

target_include_directories(engine PUBLIC
    "${PROJECT_SOURCE_DIR}"
    "thirdparty/xxhash/include"
)

target_link_directories(engine PUBLIC
)

target_link_libraries(engine PUBLIC
    simdjson
    tinygltf
    png_static
    platform
    D3D12MemoryAllocator
)

target_compile_definitions(engine PUBLIC 
    IONENGINE_RHI_DIRECTX12
)

add_executable(project 
    "tests/main.cpp"
)

target_precompile_headers(project PRIVATE precompiled.h)

target_include_directories(project PRIVATE
    "${PROJECT_SOURCE_DIR}"
)

target_link_libraries(project PUBLIC
    engine
)

add_executable(shaderm 
    "tools/shaderm/main.cpp"
)

target_precompile_headers(shaderm PRIVATE precompiled.h)

target_include_directories(shaderm PRIVATE
    "${PROJECT_SOURCE_DIR}"
)

target_link_libraries(shaderm PUBLIC
    engine
    libwebview_edge_static
    base64pp
)

add_executable(shaderc
    "tools/shaderc/main.cpp"
)

target_precompile_headers(shaderc PRIVATE precompiled.h)

target_include_directories(shaderc PRIVATE
    "${PROJECT_SOURCE_DIR}"
)