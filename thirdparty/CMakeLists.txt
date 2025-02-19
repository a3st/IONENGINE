cmake_minimum_required(VERSION 3.25.1)

include(FetchContent)

FetchContent_Declare(libwebview
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/thirdparty/libwebview
    BINARY_DIR ${CMAKE_BINARY_DIR}/thirdparty/libwebview)

FetchContent_MakeAvailable(
	libwebview)