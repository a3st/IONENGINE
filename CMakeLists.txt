cmake_minimum_required(VERSION 3.26.0)
project(IONENGINE VERSION 0.0.5)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(thirdparty/webgpu)
add_subdirectory(thirdparty/simdjson)

add_library(ionengine SHARED
    "platform/window_loop.cpp"
    "platform/window_win32.cpp"

    "math/color.cpp"

    "asset/ktx2_image.cpp"
    "asset/glb_model.cpp"

    "asset/asset_manager.cpp"

    "renderer/backend.cpp"
    "renderer/texture.cpp"
    "renderer/buffer.cpp"
    "renderer/shader.cpp"
    "renderer/drawable.cpp"
    "renderer/mesh.cpp"
    "renderer/camera.cpp"
    "renderer/upload_manager.cpp"
    "renderer/texture_cache.cpp"
    "renderer/render_graph.cpp"
    "renderer/renderer.cpp"

    "renderer/pipelines/my_render_pipeline.cpp"

    "bindings/python/test.cpp"
)

target_precompile_headers(ionengine PRIVATE precompiled.h)

target_include_directories(ionengine PRIVATE
    "${PROJECT_SOURCE_DIR}"
    "thirdparty/xxhash/include"
    "C:/Program Files/Python311/include"
)

target_link_directories(ionengine PRIVATE
    "C:/Program Files/Python311/libs"
)

target_link_libraries(ionengine
    webgpu
    simdjson
    python311.lib
)

target_compile_definitions(ionengine PRIVATE 

)