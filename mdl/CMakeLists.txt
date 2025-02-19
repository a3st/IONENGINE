cmake_minimum_required(VERSION 3.25.1)

FetchContent_Declare(tinyobjloader
    GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git
    GIT_TAG 4464aa5b401f26330583fd995f7ba6df3592653a
    BINARY_DIR ${CMAKE_BINARY_DIR}/thirdparty/tinyobjloader)

FetchContent_MakeAvailable(
    tinyobjloader)

add_library(mdl STATIC
    obj/obj.cpp
    mdl.cpp)

target_include_directories(mdl PUBLIC 
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/mdl)

target_link_libraries(mdl PUBLIC 
    core
    tinyobjloader)

target_precompile_headers(mdl PUBLIC ${PROJECT_SOURCE_DIR}/precompiled.h)