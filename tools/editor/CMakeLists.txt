cmake_minimum_required(VERSION 3.26.0)

add_executable(editor
    components/texture2d.cpp
    components/sampler2d.hpp
    components/output.cpp
    connection.cpp
    node.cpp
    scene.cpp
    main.cpp)

target_include_directories(editor PRIVATE ${PROJECT_SOURCE_DIR})

target_precompile_headers(editor PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

target_link_libraries(editor PUBLIC
    base64pp
    argh
    libwebview_edge_static)

set_target_properties(editor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tools)