cmake_minimum_required(VERSION 3.26.0)
project(IONENGINE VERSION 0.0.5)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_subdirectory(thirdparty/simdjson)
add_subdirectory(thirdparty/pybind11)
add_subdirectory(thirdparty/tinygltf)

if (WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif ()

add_library(engine SHARED
    "platform/windows/window.cpp"
    "platform/window.cpp"

    "math/color.cpp"

    "rhi/dx12/rhi.cpp"
    "rhi/rhi.cpp"
    "rhi/shader_file.cpp"

    "engine/job_system.cpp"
    "engine/material.cpp"
    "engine/model.cpp"
    "engine/engine.cpp"

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
)

target_compile_definitions(engine PRIVATE 
    IONENGINE_PLATFORM_WIN32
    IONENGINE_RHI_DIRECTX12
)

add_executable(project 
    "engine/main.cpp"
)

target_include_directories(project PRIVATE
    "${PROJECT_SOURCE_DIR}"
)

target_precompile_headers(project PRIVATE precompiled.h)

target_link_libraries(project PUBLIC
    rhi-test
)

add_library(rhi-test SHARED
    "platform/windows/window.cpp"
    "platform/window.cpp"

    "math/color.cpp"

    "rhi/dx12/rhi.cpp"
    "rhi/rhi.cpp"
    "rhi/shader_file.cpp"
)

target_precompile_headers(rhi-test PRIVATE precompiled.h)

target_include_directories(rhi-test PUBLIC
    "${PROJECT_SOURCE_DIR}"
    "thirdparty/xxhash/include"
)

target_link_libraries(rhi-test PUBLIC
    simdjson
)

target_compile_definitions(rhi-test PRIVATE 
    IONENGINE_PLATFORM_WIN32
    IONENGINE_RHI_DIRECTX12
)