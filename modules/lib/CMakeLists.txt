cmake_minimum_required(VERSION 3.0.0)

add_library(Lib
    algorithm.cpp
    logger.cpp
    thread_pool.cpp
    math/color.cpp
)

target_include_directories(Lib PRIVATE
    ${PROJECT_SOURCE_DIR}/modules
    ${PROJECT_SOURCE_DIR}/thirdparty/xxhash/include
    ${PROJECT_SOURCE_DIR}/thirdparty/range-v3/include
)

target_precompile_headers(Lib PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)