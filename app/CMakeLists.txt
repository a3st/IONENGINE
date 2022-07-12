cmake_minimum_required(VERSION 3.0.0)

add_executable(App
    app.cpp
)

target_precompile_headers(App PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

#target_include_directories(App PRIVATE)

target_link_libraries(App PRIVATE 
    Platform 
    Renderer 
    Math
)