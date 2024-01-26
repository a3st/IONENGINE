cmake_minimum_required(VERSION 3.26.0)
project(IONENGINE VERSION 0.0.5)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(thirdparty/simdjson)
add_subdirectory(thirdparty/pybind11)

if (WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif ()

add_library(engine SHARED
    "platform/window_loop.cpp"
    "platform/window_win32.cpp"

    "math/color.cpp"

    "asset/ktx2_image.cpp"
    "asset/glb_model.cpp"
    "asset/model.cpp"
    "asset/asset_manager.cpp"

    "renderer/rhi/dx12/allocator.cpp"
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

    "renderer/cache/rg_resource_cache.cpp"
    "renderer/cache/primitive_cache.cpp"
    #"renderer/camera.cpp"
    #"renderer/primitive_cache.cpp"
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