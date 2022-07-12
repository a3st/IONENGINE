cmake_minimum_required(VERSION 3.0.0)

project(VKMemAlloc VERSION 2.0.0)

include_directories(${PROJECT_SOURCE_DIR}/include/vma)

set(D3D12MA_LIBRARY_SOURCE_FILES
    "${PROJECT_SOURCE_DIR}/src/vk_mem_alloc.cpp"
    "${PROJECT_SOURCE_DIR}/include/vma/vk_mem_alloc.h"
)

add_library(VulkanMemoryAllocator ${D3D12MA_LIBRARY_SOURCE_FILES})

set_target_properties(
    VulkanMemoryAllocator PROPERTIES

    CXX_EXTENSIONS OFF
    # Use C++14
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON

    OUTPUT_NAME "VMA"
)

target_include_directories(VulkanMemoryAllocator PRIVATE
    $ENV{VULKAN_SDK}/Include
)

target_link_directories(VulkanMemoryAllocator PRIVATE
    $ENV{VULKAN_SDK}/Lib
)

target_link_libraries(VulkanMemoryAllocator PRIVATE
    vulkan-1.lib
)
