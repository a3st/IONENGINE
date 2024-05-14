cmake_minimum_required(VERSION 3.26.0)

add_executable(shaderc
    ../../rhi/fx/fxsl.cpp
    compiler.cpp
    fx.cpp
    main.cpp
)

target_include_directories(shaderc PRIVATE ${PROJECT_SOURCE_DIR})

target_precompile_headers(shaderc PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

target_link_libraries(shaderc PUBLIC
    dxgi
    dxcompiler
    simdjson
    argh
)