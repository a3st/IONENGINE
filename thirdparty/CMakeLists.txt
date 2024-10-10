cmake_minimum_required(VERSION 3.25.1)

include(FetchContent)

FetchContent_Declare(libwebview
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/thirdparty/libwebview
    BINARY_DIR ${CMAKE_BINARY_DIR}/thirdparty/libwebview)

FetchContent_Declare(tinyobjloader
    GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git
    GIT_TAG 4464aa5b401f26330583fd995f7ba6df3592653a
    BINARY_DIR ${CMAKE_BINARY_DIR}/thirdparty/tinyobjloader)

FetchContent_MakeAvailable(
	libwebview
    tinyobjloader)