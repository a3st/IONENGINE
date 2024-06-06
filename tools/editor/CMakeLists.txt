cmake_minimum_required(VERSION 3.26.0)

add_executable(editor
    components/convert.hpp
    components/texture.hpp
    components/output.cpp
    components/input.cpp
    view_model.cpp
    connection.cpp
    registry.cpp
    node.cpp
    scene.cpp
    main.cpp)

target_include_directories(editor PRIVATE ${PROJECT_SOURCE_DIR})

target_precompile_headers(editor PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

target_link_libraries(editor PUBLIC
    base64pp
    argh
    libwebview_edge_static
    engine)

set_target_properties(editor PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tools
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tools
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tools)

message(WARNING ${CMAKE_CURRENT_BINARY_DIR}/engine.dll)

add_custom_command(TARGET editor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/engine/engine.dll ${CMAKE_CURRENT_BINARY_DIR}/../engine.dll
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/engine/engine.pdb ${CMAKE_CURRENT_BINARY_DIR}/../engine.pdb)