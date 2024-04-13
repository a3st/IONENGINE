cmake_minimum_required(VERSION 3.26.0)

add_executable(rhi_fx_test 
    rhi/fx/main.cpp
    ../rhi/fx/fxsl.cpp
    ../rhi/fx/dxc/compiler.cpp
)

target_include_directories(rhi_fx_test PRIVATE
    "${PROJECT_SOURCE_DIR}"
)

target_link_libraries(rhi_fx_test
    gtest
    simdjson
)

target_precompile_headers(rhi_fx_test PRIVATE "${PROJECT_SOURCE_DIR}/precompiled.h")

add_test(NAME rhi_fx_test COMMAND rhi_fx_test)