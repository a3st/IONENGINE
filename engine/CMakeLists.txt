cmake_minimum_required(VERSION 3.0.0)

add_library(Engine
    framework.cpp
)

target_include_directories(Engine PRIVATE 
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/thirdparty/range-v3/include
)

target_precompile_headers(Engine PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

target_link_libraries(Engine
    Lib
    Platform
)