cmake_minimum_required(VERSION 3.26.0)
project(IONENGINE VERSION 0.0.5)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(thirdparty/webgpu)
add_subdirectory(thirdparty/simdjson)
add_subdirectory(thirdparty/pybind11)

add_library(engine SHARED
    "platform/window_loop.cpp"
    "platform/window_win32.cpp"

    "math/color.cpp"

    "asset/ktx2_image.cpp"
    "asset/glb_model.cpp"
    "asset/model.cpp"
    "asset/asset_manager.cpp"

    "renderer/context.cpp"
    "renderer/texture.cpp"
    "renderer/buffer.cpp"
    "renderer/shader.cpp"
    "renderer/primitive.cpp"
    "renderer/camera.cpp"
    "renderer/texture_cache.cpp"
    "renderer/primitive_cache.cpp"
    "renderer/render_graph.cpp"
    "renderer/renderer.cpp"

    "renderer/pipelines/my_render_pipeline.cpp"

    "engine/engine.cpp"

    "bindings/python/engine.cpp"
)

target_precompile_headers(engine PRIVATE precompiled.h)

target_include_directories(engine PRIVATE
    "${PROJECT_SOURCE_DIR}"
    "thirdparty/xxhash/include"
    "C:/Program Files/Python311/include"
)

target_link_directories(engine PRIVATE
    "C:/Program Files/Python311/libs"
)

target_link_libraries(engine
    webgpu
    simdjson
    pybind11::headers
    python311.lib
)

target_compile_definitions(engine PRIVATE 

)