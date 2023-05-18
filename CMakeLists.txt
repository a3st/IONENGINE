cmake_minimum_required(VERSION 3.0.0)

project(IONENGINE VERSION 1.0.0)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)

include_directories("${PROJECT_SOURCE_DIR}/include")

add_executable(Win32-Runtime
    "src/main.cpp"

    "src/platform/win32/window.cpp"
    "src/gapi/d3d12/gapi.cpp"
    "src/gapi/d3d12/allocator.cpp"

    "include/gapi/gapi.hpp"
    "include/platform/window.hpp"
)

target_link_libraries(Win32-Runtime PRIVATE
    "dwmapi.lib"
    "Shcore.lib"
    "dxgi.lib"
    "d3d12.lib"
)

target_precompile_headers(Win32-Runtime PRIVATE "${PROJECT_SOURCE_DIR}/include/precompiled.h")