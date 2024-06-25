cmake_minimum_required(VERSION 3.26.0)
project(IONENGINE VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if(NOT TARGET simdjson)
add_subdirectory(thirdparty/simdjson)
endif()

if(NOT TARGET tinygltf)
add_subdirectory(thirdparty/tinygltf)
endif()

if(NOT TARGET libpng)
add_subdirectory(thirdparty/libpng)
endif()

if(NOT TARGET libwebview)
add_subdirectory(thirdparty/libwebview)
endif()

if(NOT TARGET base64pp)
add_subdirectory(thirdparty/base64pp)
endif()

if(NOT TARGET D3D12MemoryAllocator)
add_subdirectory(thirdparty/d3d12ma)
endif()

if(NOT TARGET gtest)
add_subdirectory(thirdparty/googletest)
endif()

if(NOT TARGET argh)
add_subdirectory(thirdparty/argh)
endif()

add_subdirectory(tools)
add_subdirectory(engine)
add_subdirectory(tests)