cmake_minimum_required(VERSION 3.26.0)

add_library(engine SHARED
    ../math/color.cpp

    ../rhi/dx12/rhi.cpp
    ../rhi/rhi.cpp
    ../rhi/fx/fxsl.cpp

    ../platform/windows/window.cpp
    ../platform/window.cpp

    importers/glb.cpp

    renderer/linked_device.cpp
    renderer/texture.cpp
    renderer/shader.cpp
    renderer/renderer.cpp

    engine.cpp)

target_include_directories(engine PRIVATE 
    ${PROJECT_SOURCE_DIR} 
    ${PROJECT_SOURCE_DIR}/thirdparty/xxhash/include)

target_precompile_headers(engine PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

target_link_libraries(engine PUBLIC
    dxgi
    d3d12
    simdjson
    tinygltf
    png_static
    D3D12MemoryAllocator)

target_compile_definitions(engine PUBLIC
    IONENGINE_PLATFORM_WIN32
    IONENGINE_RHI_DIRECTX12)

set_target_properties(engine PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/engine
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/engine
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/engine)