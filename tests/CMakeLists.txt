cmake_minimum_required(VERSION 3.25.1)

# Serialize
add_executable(serialize_test serialize_test.cpp)

target_include_directories(serialize_test PRIVATE ${PROJECT_SOURCE_DIR})

target_link_libraries(serialize_test PRIVATE
    simdjson
    gtest)

target_precompile_headers(serialize_test PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

# Base64
add_executable(base64_test base64_test.cpp)

target_include_directories(base64_test PRIVATE ${PROJECT_SOURCE_DIR})

target_link_libraries(base64_test PRIVATE
    gtest)

target_precompile_headers(base64_test PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

# Platform
add_executable(platform_test platform_test.cpp)

target_include_directories(platform_test PRIVATE ${PROJECT_SOURCE_DIR})

target_link_libraries(platform_test PRIVATE 
    gtest
    platform)

target_precompile_headers(platform_test PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

# RHI
add_executable(rhi_test rhi_test.cpp)

target_include_directories(rhi_test PRIVATE ${PROJECT_SOURCE_DIR})

target_link_libraries(rhi_test PRIVATE 
    platform
    gtest
    rhi)

target_precompile_headers(rhi_test PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)