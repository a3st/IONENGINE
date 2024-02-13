cmake_minimum_required(VERSION 3.26.0)
project(IONENGINE VERSION 0.0.5)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_subdirectory(thirdparty/simdjson)
add_subdirectory(thirdparty/pybind11)

if (WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif ()

add_library(engine SHARED
    "platform/windows/window.cpp"
    "platform/window_loop.cpp"
    "platform/window.cpp"

    "math/color.cpp"

    "core/job_system.cpp"

    "asset/importers/ktx2_image.cpp"
    "asset/importers/glb_model.cpp"
    
    "asset/model.cpp"
    "asset/shader.cpp"
    "asset/asset_manager.cpp"

    "renderer/rhi/dx12/barrier_proxy.cpp"
    "renderer/rhi/dx12/memory_allocator.cpp"
    "renderer/rhi/dx12/descriptor_allocator.cpp"
    "renderer/rhi/dx12/command_allocator.cpp"
    "renderer/rhi/dx12/upload_context.cpp"
    "renderer/rhi/dx12/pipeline_cache.cpp"
    "renderer/rhi/dx12/texture.cpp"
    "renderer/rhi/dx12/buffer.cpp"
    "renderer/rhi/dx12/shader.cpp"
    "renderer/rhi/dx12/command_buffer.cpp"
    "renderer/rhi/dx12/future.cpp"
    "renderer/rhi/dx12/device.cpp"
    "renderer/rhi/device.cpp"
    "renderer/rhi/shader.cpp"

    "renderer/buffer_pool.cpp"
    "renderer/render_target.cpp"
    "renderer/primitive.cpp"
    "renderer/texture.cpp"
    "renderer/camera.cpp"
    "renderer/shader.cpp"
    "renderer/render_graph.cpp"
    "renderer/renderer.cpp"

    "renderer/pipelines/my_render_pipeline.cpp"

    "engine/engine.cpp"

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
)

target_compile_definitions(engine PRIVATE 
    IONENGINE_PLATFORM_WIN32
    IONENGINE_RENDERER_DIRECTX12
)

add_executable(project 
    "engine/main.cpp"
)

target_include_directories(project PRIVATE
    "${PROJECT_SOURCE_DIR}"
)

target_precompile_headers(project PRIVATE precompiled.h)

target_link_libraries(project PUBLIC
    engine
)