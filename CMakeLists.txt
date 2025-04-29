cmake_minimum_required(VERSION 3.25.1)

project(IONENGINE VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

option(BUILD_TESTING "Build unit-tests" TRUE)

add_subdirectory(core)
add_subdirectory(platform)
add_subdirectory(rhi)
add_subdirectory(mdl)
add_subdirectory(txe)
add_subdirectory(shadersys)
add_subdirectory(engine)
add_subdirectory(tools)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()