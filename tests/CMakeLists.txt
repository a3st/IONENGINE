cmake_minimum_required(VERSION 3.26.0)

add_executable(serializable_test
    serializable_test.cpp)

target_include_directories(serializable_test PRIVATE 
    ${PROJECT_SOURCE_DIR})

target_link_libraries(serializable_test PUBLIC
    simdjson
    gtest)

target_precompile_headers(serializable_test PRIVATE 
    ${PROJECT_SOURCE_DIR}/precompiled.h)