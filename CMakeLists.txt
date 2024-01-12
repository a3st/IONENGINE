cmake_minimum_required(VERSION 3.0.0)
project(IONENGINE VERSION 0.0.5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(thirdparty/webgpu)

add_executable(Runtime-Win32
    "platform/window_loop.cpp"
    "platform/window_win32.cpp"

    "math/color.cpp"

    "renderer/backend.cpp"
    "renderer/texture.cpp"
    "renderer/mesh.cpp"
    "renderer/shader.cpp"
    "renderer/camera.cpp"
    "renderer/render_graph.cpp"
    "renderer/renderer.cpp"
    
    "engine/main.cpp"
)

target_precompile_headers(Runtime-Win32 PRIVATE precompiled.h)

set_target_properties(Runtime-Win32 PROPERTIES 
    OUTPUT_NAME "project"
    COMPILE_FLAGS "${COMPILE_FLAGS}"
)

target_include_directories(Runtime-Win32 PRIVATE
    "${PROJECT_SOURCE_DIR}"
    "thirdparty/xxhash/include"
)

target_link_directories(Runtime-Win32 PRIVATE
)

target_link_libraries(Runtime-Win32
    webgpu
)

target_compile_definitions(Runtime-Win32 PRIVATE 

)