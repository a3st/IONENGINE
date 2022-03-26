cmake_minimum_required(VERSION 3.0.0)

project(D3D12MemAlloc VERSION 2.0.0)

include_directories(${PROJECT_SOURCE_DIR}/include/d3d12_ma)

set(D3D12MA_LIBRARY_SOURCE_FILES
    "${PROJECT_SOURCE_DIR}/src/D3D12MemAlloc.cpp"
    "${PROJECT_SOURCE_DIR}/include/d3d12_ma/D3D12MemAlloc.h"
)

add_library(D3D12MemoryAllocator ${D3D12MA_LIBRARY_SOURCE_FILES})

set_target_properties(
    D3D12MemoryAllocator PROPERTIES

    CXX_EXTENSIONS OFF
    # Use C++14
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON

    OUTPUT_NAME "D3D12MA"
)

target_link_libraries(D3D12MemoryAllocator PUBLIC
    d3d12.lib
    dxgi.lib
    dxguid.lib
)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(D3D12MemoryAllocator PRIVATE
        D3D12MA_EXPORTS
    )

    target_compile_definitions(D3D12MemoryAllocator INTERFACE
        D3D12MA_IMPORTS
    )
endif()
