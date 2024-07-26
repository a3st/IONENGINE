cmake_minimum_required(VERSION 3.25.1)
include(FetchContent)

set(RHI_TARGET_BACKEND "DX12" CACHE STRING "RHI target backend")

add_library(rhi STATIC rhi.cpp)

target_include_directories(rhi PUBLIC 
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/rhi)

target_precompile_headers(rhi PUBLIC ${PROJECT_SOURCE_DIR}/precompiled.h)

if(RHI_TARGET_BACKEND STREQUAL "DX12")
    if(WIN32)
        target_sources(rhi PRIVATE dx12/dx12.cpp)
        
        find_package(xxHash CONFIG REQUIRED)
        find_package(d3d12-memory-allocator CONFIG REQUIRED)

        target_link_libraries(rhi PUBLIC 
            unofficial::D3D12MemoryAllocator
            xxHash::xxhash
            d3d12
            dxgi)

        target_compile_definitions(rhi PUBLIC 
            IONENGINE_RHI_DIRECTX12)
    else()
        message(FATAL_ERROR "DirectX12 is available only on Windows")
    endif()
elseif(RHI_TARGET_BACKEND STREQUAL "VK")
    if(WIN32)
        message(FATAL_ERROR "Vulkan is not currently supported")
    elseif(UNIX)
        target_sources(rhi PRIVATE vulkan/vk.cpp)

        find_package(PkgConfig REQUIRED)
        pkg_check_modules(VK REQUIRED vulkan)
        
        target_include_directories(rhi PUBLIC
            ${VK_INCLUDE_DIRS})
        
        target_link_directories(rhi PUBLIC
            ${VK_LIBRARY_DIRS})
        
        target_link_libraries(rhi PUBLIC
            VulkanMemoryAllocator
            ${VK_LIBRARIES})
        
        target_compile_definitions(rhi PUBLIC
            IONENGINE_RHI_VULKAN
            _DEBUG)
    else()
        message(FATAL_ERROR "Vulkan is not available on this platform")
    endif()
endif()