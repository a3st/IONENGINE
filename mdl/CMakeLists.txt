cmake_minimum_required(VERSION 3.25.1)

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