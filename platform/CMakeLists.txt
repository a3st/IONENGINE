cmake_minimum_required(VERSION 3.25.1)

add_library(platform STATIC platform.cpp)

target_include_directories(platform PUBLIC
    ${PROJECT_SOURCE_DIR} 
    ${PROJECT_SOURCE_DIR}/platform
)

target_precompile_headers(platform PUBLIC ${PROJECT_SOURCE_DIR}/precompiled.h)

if(UNIX)
target_sources(platform PRIVATE x11/x11.cpp)

find_package(PkgConfig REQUIRED)
pkg_check_modules(X11 REQUIRED x11)

target_include_directories(platform PUBLIC
    ${X11_INCLUDE_DIRS}
)

target_link_directories(platform PUBLIC
    ${X11_LIBRARY_DIRS}
)

target_link_libraries(platform PUBLIC
    ${X11_LIBRARIES}
)

target_compile_definitions(platform PUBLIC
    IONENGINE_PLATFORM_X11
)
endif()