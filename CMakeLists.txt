cmake_minimum_required(VERSION 3.0.0)

project(IONENGINE VERSION 0.1.0)

# Shared includes
include_directories(${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++17 -stdlib=libc++ -arch x86_x64")


#[[

    Windows Runtime Build Target

#]]

add_executable(Runtime framework/main_win32.cpp)
target_precompile_headers(Runtime PRIVATE precompiled.h)

# Build Output
set_target_properties(Runtime PROPERTIES 
    OUTPUT_NAME "IONENGINE-Runtime"
)

target_include_directories(Runtime PRIVATE 
    "C:/Program Files (x86)/Windows Kits/10/Include/10.0.18362.0/um"
    "C:/Program Files (x86)/Windows Kits/10/Include/10.0.18362.0/winrt"
    "C:/Program Files (x86)/Windows Kits/10/Include/10.0.18362.0/"
)

# Preprocessor
target_compile_definitions(Runtime PRIVATE 
    IONENGINE_PLATFORM_WIN32
    RENDERER_API_D3D12
)