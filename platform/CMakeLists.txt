cmake_minimum_required(VERSION 3.0.0)

add_library(Platform
    src/window_glfw.cpp
)

target_precompile_headers(Platform PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

target_include_directories(Platform PUBLIC
    ${PROJECT_SOURCE_DIR}/platform/include
    ${PROJECT_SOURCE_DIR}/thirdparty/glfw/include
)

target_link_directories(Platform PUBLIC
    ${PROJECT_SOURCE_DIR}/thirdparty/glfw/build/${CMAKE_BUILD_TYPE}
)

target_link_libraries(Platform
    glfw3.lib
)
