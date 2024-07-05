cmake_minimum_required(VERSION 3.26.0)

add_executable(editor
    graph/connection.cpp
    graph/registry.cpp
    graph/node.cpp
    graph/scene.cpp

    conv/texture.cpp
    
    shader_graph/components/convert.cpp
    shader_graph/components/texture.cpp
    shader_graph/components/output.cpp
    shader_graph/components/input.cpp
    shader_graph/components/math.cpp
    shader_graph/shader_graph.cpp
    shader_graph/editor.cpp

    view_model.cpp
    asset_tree.cpp
    main.cpp)

target_include_directories(editor PRIVATE 
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/tools/editor)

target_precompile_headers(editor PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

target_link_libraries(editor PUBLIC
    argh
    libwebview_edge_static
    engine)

set_target_properties(editor PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tools
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tools
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tools)

add_custom_command(TARGET editor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/engine/engine.dll ${CMAKE_CURRENT_BINARY_DIR}/../engine.dll
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/engine/engine.pdb ${CMAKE_CURRENT_BINARY_DIR}/../engine.pdb)