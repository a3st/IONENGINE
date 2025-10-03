cmake_minimum_required(VERSION 3.25.1)

find_package(simdjson CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

# RHI
add_executable(rhi_test rhi_test.cpp)

target_include_directories(rhi_test PRIVATE ${PROJECT_SOURCE_DIR})

target_link_libraries(rhi_test PRIVATE 
    platform
    GTest::gtest
    rhi
    shadersys
    mdl
    txe)

target_precompile_headers(rhi_test PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

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

# TXE
add_executable(txe_test txe_test.cpp)

target_include_directories(txe_test PRIVATE ${PROJECT_SOURCE_DIR})

target_link_libraries(txe_test PRIVATE 
    txe
    GTest::gtest)

target_precompile_headers(txe_test PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)