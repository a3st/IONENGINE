cmake_minimum_required(VERSION 3.0.0)

project(IONENGINE VERSION 1.0.0)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(platform)
