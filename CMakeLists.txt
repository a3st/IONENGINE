cmake_minimum_required(VERSION 3.26.0)
project(IONENGINE VERSION 0.0.5)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_subdirectory(thirdparty/simdjson)
add_subdirectory(thirdparty/pybind11)
add_subdirectory(thirdparty/tinygltf)
add_subdirectory(thirdparty/libpng)

if (WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif ()

add_library(platform SHARED
    "platform/windows/window.cpp"
    "platform/window.cpp"

    "bindings/python/platform.cpp"
)

target_precompile_headers(platform PRIVATE precompiled.h)

target_include_directories(platform PUBLIC
    "${PROJECT_SOURCE_DIR}"
    "C:/Program Files/Python311/include"
)

target_link_directories(platform PUBLIC
    "C:/Program Files/Python311/libs"
)

target_link_libraries(platform PUBLIC
    pybind11::headers
    python311.lib
)

target_compile_definitions(platform PUBLIC 
    IONENGINE_PLATFORM_WIN32
)

add_library(engine SHARED
    "math/color.cpp"

    "rhi/dx12/rhi.cpp"
    "rhi/rhi.cpp"
    "rhi/shader_file.cpp"

    "engine/job_system.cpp"
    "engine/material.cpp"
    "engine/model.cpp"
    "engine/texture.cpp"
    "engine/engine.cpp"
    "engine/linked_device.cpp"
    "engine/asset_loader.cpp"
    "engine/renderer.cpp"
    "engine/shader_manager.cpp"

    "extensions/importers/glb.cpp"

    "bindings/python/engine.cpp"
)

target_precompile_headers(engine PRIVATE precompiled.h)

target_include_directories(engine PUBLIC
    "${PROJECT_SOURCE_DIR}"
    "thirdparty/xxhash/include"
    "C:/Program Files/Python311/include"
)

target_link_directories(engine PUBLIC
    "C:/Program Files/Python311/libs"
)

target_link_libraries(engine PUBLIC
    simdjson
    pybind11::headers
    python311.lib
    tinygltf
    png_static
    platform
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