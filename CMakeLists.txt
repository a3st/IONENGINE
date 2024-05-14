cmake_minimum_required(VERSION 3.26.0)
project(IONENGINE VERSION 0.0.5)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_subdirectory(thirdparty/simdjson)
add_subdirectory(thirdparty/tinygltf)
add_subdirectory(thirdparty/libpng)
add_subdirectory(thirdparty/libwebview)
add_subdirectory(thirdparty/base64pp)
add_subdirectory(thirdparty/d3d12ma)
add_subdirectory(thirdparty/googletest)
add_subdirectory(thirdparty/argh)

add_subdirectory(tools)