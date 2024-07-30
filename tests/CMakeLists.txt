cmake_minimum_required(VERSION 3.25.1)

find_package(simdjson CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

# Serialize
add_executable(serialize_test serialize_test.cpp)

target_include_directories(serialize_test PRIVATE ${PROJECT_SOURCE_DIR})

target_link_libraries(serialize_test PRIVATE
    simdjson::simdjson
    GTest::gtest)

target_precompile_headers(serialize_test PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

# Base64
add_executable(base64_test base64_test.cpp)

target_include_directories(base64_test PRIVATE ${PROJECT_SOURCE_DIR})

target_link_libraries(base64_test PRIVATE
    GTest::gtest)

target_precompile_headers(base64_test PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

# Platform
add_executable(platform_test platform_test.cpp)

target_include_directories(platform_test PRIVATE ${PROJECT_SOURCE_DIR})

target_link_libraries(platform_test PRIVATE 
    GTest::gtest
    platform)

target_precompile_headers(platform_test PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

# RHI
add_executable(rhi_test rhi_test.cpp)

target_include_directories(rhi_test PRIVATE ${PROJECT_SOURCE_DIR})

target_link_libraries(rhi_test PRIVATE 
    platform
    GTest::gtest
    rhi)

target_precompile_headers(rhi_test PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

# shaderc
add_executable(shaderc_test
    ../rhi/fx/fx.cpp
    ../tools/shaderc/lexer.cpp
    ../tools/shaderc/parser.cpp
    shaderc_test.cpp)

target_include_directories(shaderc_test PRIVATE ${PROJECT_SOURCE_DIR})

target_link_libraries(shaderc_test PRIVATE 
    GTest::gtest)

target_precompile_headers(shaderc_test PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)