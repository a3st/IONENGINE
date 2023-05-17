
cmake_minimum_required(VERSION 3.0.0)

add_library(platform 
    impl/window_win32.cpp
    window.h


)

target_precompile_headers(platform PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

target_include_directories(platform PUBLIC
)