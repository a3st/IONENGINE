cmake_minimum_required(VERSION 3.25.1)
include(FetchContent)

add_library(rhi STATIC rhi.cpp)

target_include_directories(rhi PUBLIC
    ${PROJECT_SOURCE_DIR} 
    ${PROJECT_SOURCE_DIR}/rhi)

target_precompile_headers(rhi PUBLIC ${PROJECT_SOURCE_DIR}/precompiled.h)

if(UNIX)
FetchContent_Declare(VulkanMemoryAllocator SYSTEM
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/thirdparty/vma
    BINARY_DIR ${CMAKE_BINARY_DIR}/thirdparty/vma)

FetchContent_MakeAvailable(VulkanMemoryAllocator)

target_sources(rhi PRIVATE vulkan/rhi.cpp)

find_package(PkgConfig REQUIRED)
pkg_check_modules(VK REQUIRED vulkan)

target_include_directories(rhi PUBLIC
    ${VK_INCLUDE_DIRS}
)

target_link_directories(rhi PUBLIC
    ${VK_LIBRARY_DIRS}
)

target_link_libraries(rhi PUBLIC
    VulkanMemoryAllocator
    ${VK_LIBRARIES}
)

target_compile_definitions(rhi PUBLIC
    IONENGINE_RHI_VULKAN
    _DEBUG)
endif()