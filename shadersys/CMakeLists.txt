cmake_minimum_required(VERSION 3.25.1)

add_library(shadersys STATIC
    lexer.cpp
    parser.cpp
    fx.cpp
    compiler.cpp
    dxc/dxc.cpp)

target_include_directories(shadersys PUBLIC 
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/shadersys)

target_include_directories(shadersys PUBLIC 
    ${PROJECT_SOURCE_DIR}/thirdparty/dxc/windows/include)

target_link_directories(shadersys PUBLIC
    ${PROJECT_SOURCE_DIR}/thirdparty/dxc/windows/lib/x64)

target_link_libraries(shadersys PUBLIC 
    core
    dxgi
    dxcompiler)

target_compile_definitions(shadersys PUBLIC
    IONENGINE_SHADERSYS_DXC)

target_precompile_headers(shadersys PUBLIC ${PROJECT_SOURCE_DIR}/precompiled.h)