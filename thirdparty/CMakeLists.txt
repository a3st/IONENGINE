cmake_minimum_required(VERSION 3.25.1)
include(FetchContent)

FetchContent_Declare(zlib
    GIT_REPOSITORY https://github.com/madler/zlib.git
    BINARY_DIR ${CMAKE_BINARY_DIR}/thirdparty/zlib)

FetchContent_Declare(
    libpng
    GIT_REPOSITORY https://github.com/pnggroup/libpng.git
    BINARY_DIR ${CMAKE_BINARY_DIR}/thirdparty/libpng)

FetchContent_Declare(simdjson
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/thirdparty/simdjson
    BINARY_DIR ${CMAKE_BINARY_DIR}/thirdparty/simdjson)

FetchContent_Declare(
    tinygltf
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/thirdparty/tinygltf
    BINARY_DIR ${CMAKE_BINARY_DIR}/thirdparty/tinygltf)

FetchContent_Declare(
    libwebview
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/thirdparty/libwebview
    BINARY_DIR ${CMAKE_BINARY_DIR}/thirdparty/libwebview)

FetchContent_Declare(
    D3D12MemoryAllocator
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/thirdparty/d3d12ma
    BINARY_DIR ${CMAKE_BINARY_DIR}/thirdparty/d3d12ma)

FetchContent_Declare(
    gtest
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/thirdparty/googletest
    BINARY_DIR ${CMAKE_BINARY_DIR}/thirdparty/googletest)

FetchContent_Declare(
    argh
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/thirdparty/argh
    BINARY_DIR ${CMAKE_BINARY_DIR}/thirdparty/argh)

FetchContent_MakeAvailable(
    zlib
    libpng
    simdjson 
    tinygltf 
    libwebview  
   # D3D12MemoryAllocator 
    gtest 
    argh)