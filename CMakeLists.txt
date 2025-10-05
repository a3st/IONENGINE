cmake_minimum_required(VERSION 3.25.1)

project(IONENGINE 
    VERSION 1.0.0 
    DESCRIPTION "Graphics engine that written in modern C++"
    LANGUAGES CXX
    HOMEPAGE_URL https://github.com/a3st/IONENGINE
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(SUPPORTED_ARCHITECTURES "x64;arm64")
include(cmake/TargetArch.cmake)

option(BUILD_TESTING "Build unit-tests" TRUE)

if(BUILD_TESTING)
    enable_testing()
    find_package(GTest REQUIRED)
    include(GoogleTest)
endif()

add_subdirectory(core)
add_subdirectory(platform)
add_subdirectory(rhi)
add_subdirectory(mdl)
add_subdirectory(txe)
add_subdirectory(shadersys)
add_subdirectory(engine)
add_subdirectory(tools)