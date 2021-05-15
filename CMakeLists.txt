cmake_minimum_required(VERSION 3.0.0)
project(IONENGINE VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(IONENGINE framework/main_win32.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -stdlib=libc++")

target_precompile_headers(IONENGINE PUBLIC precompiled.h)

target_include_directories(IONENGINE PRIVATE ${PROJECT_SOURCE_DIR})

include(CPack)
