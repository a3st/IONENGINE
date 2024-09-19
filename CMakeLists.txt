cmake_minimum_required(VERSION 3.25.1)

project(IONENGINE VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

option(BUILD_TESTING "Enable Build Tests" TRUE)

add_subdirectory(thirdparty)
add_subdirectory(core)
add_subdirectory(platform)
add_subdirectory(rhi)
add_subdirectory(shadersys)

# add_subdirectory(tools)
# add_subdirectory(engine)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()