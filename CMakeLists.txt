cmake_minimum_required(VERSION 3.0.0)

project(IONENGINE VERSION 0.1.0)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(modules/lib)
add_subdirectory(modules/platform)
add_subdirectory(modules/input)
add_subdirectory(modules/renderer)
add_subdirectory(platform)
add_subdirectory(engine)
add_subdirectory(app)
