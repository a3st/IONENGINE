cmake_minimum_required(VERSION 3.25.1)

add_library(shadersys STATIC
    dxc/dxc.cpp
    compiler.cpp
    lexer.cpp
    parser.cpp
    fx.cpp
)

target_include_directories(shadersys PRIVATE ${PROJECT_SOURCE_DIR})

target_link_libraries(shadersys PUBLIC core)

target_compile_definitions(shadersys PUBLIC
    IONENGINE_SHADERSYS_DXC)

target_precompile_headers(shadersys PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)