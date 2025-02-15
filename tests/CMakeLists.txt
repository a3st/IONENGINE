cmake_minimum_required(VERSION 3.25.1)

find_package(simdjson CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

# Core
add_executable(core_test core_test.cpp)

target_include_directories(core_test PRIVATE ${PROJECT_SOURCE_DIR})

target_link_libraries(core_test PRIVATE
    simdjson::simdjson
    GTest::gtest)

target_precompile_headers(core_test PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

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
    rhi
    shadersys
    mdl)

target_precompile_headers(rhi_test PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

# Shader System
add_executable(shadersys_test shadersys_test.cpp)

target_include_directories(shadersys_test PRIVATE ${PROJECT_SOURCE_DIR})

target_link_libraries(shadersys_test PRIVATE 
    shadersys
    GTest::gtest)

target_precompile_headers(shadersys_test PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

# Engine
add_executable(engine_test engine_test.cpp)

target_include_directories(engine_test PRIVATE ${PROJECT_SOURCE_DIR})

target_link_libraries(engine_test PRIVATE 
    engine
    GTest::gtest)

target_precompile_headers(engine_test PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

# MDL
add_executable(mdl_test mdl_test.cpp)

target_include_directories(mdl_test PRIVATE ${PROJECT_SOURCE_DIR})

target_link_libraries(mdl_test PRIVATE 
    mdl
    GTest::gtest)

target_precompile_headers(mdl_test PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)