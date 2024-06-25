cmake_minimum_required(VERSION 3.26.0)
project(IONENGINE VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_subdirectory(thirdparty)
add_subdirectory(tools)
add_subdirectory(engine)
add_subdirectory(tests)