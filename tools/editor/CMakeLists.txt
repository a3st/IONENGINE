cmake_minimum_required(VERSION 3.26.0)

add_executable(editor
    components/sampler2d.hpp
    components/output.cpp
    components/input.cpp
    app.cpp
    connection.cpp
    registry.cpp
    node.cpp
    scene.cpp
    merger.cpp
    main.cpp)

target_include_directories(editor PRIVATE ${PROJECT_SOURCE_DIR})

target_precompile_headers(editor PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

target_link_libraries(editor PUBLIC
    base64pp
    argh
    libwebview_edge_static)

set_target_properties(editor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tools)