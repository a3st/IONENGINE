# Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

cmake_minimum_required(VERSION 3.0.0)

# RENDERER_BUILD_API available values: "D3D12" or "VK"

# By default on Windows platform is "D3D12"
if (WIN32)
set(RENDERER_BUILD_API "D3D12")
endif()

# By default on Linux platform is "VK"
if (UNIX AND NOT APPLE)
set(RENDERER_BUILD_API "VK")
endif()

# Override RENDERER_BUILD_API variable there:
set(RENDERER_BUILD_API "D3D12")

#
# DONT REWRITE THIS PART IF YOU DONT MODIFY ENGINE
#
set(RENDERER_SOURCE_FILES "")
set(RENDERER_INCLUDE_DIRS 
    ${PROJECT_SOURCE_DIR}/renderer/include
)
set(RENDERER_LINK_DIRS "")
set(RENDERER_LINK_LIBS 
    Platform Math
)

if (RENDERER_BUILD_API STREQUAL "D3D12")

    list(APPEND RENDERER_SOURCE_FILES 
        src/device_d3d12.cpp
        src/texture_d3d12.cpp
        src/descriptor_d3d12.cpp
    )

    list(APPEND RENDERER_INCLUDE_DIRS
        ${PROJECT_SOURCE_DIR}/thirdparty/d3d12ma/include
    )

    list(APPEND RENDERER_LINK_DIRS
        ${PROJECT_SOURCE_DIR}/thirdparty/d3d12ma/build/${CMAKE_BUILD_TYPE}
    )

    list(APPEND RENDERER_LINK_LIBS
        d3d12.lib
        dxgi.lib
        D3D12MA.lib
    )

elseif (RENDERER_BUILD_API STREQUAL "VK")

    list(APPEND RENDERER_SOURCE_FILES 
        src/device_vk.cpp
        src/texture_vk.cpp
    )

    list(APPEND RENDERER_INCLUDE_DIRS
        ${PROJECT_SOURCE_DIR}/thirdparty/vma/include
        $ENV{VULKAN_SDK}/Include
    )

    list(APPEND RENDERER_LINK_DIRS
        ${PROJECT_SOURCE_DIR}/thirdparty/vma/build/${CMAKE_BUILD_TYPE}
        $ENV{VULKAN_SDK}/Lib
    )

    list(APPEND RENDERER_LINK_LIBS
        vulkan-1.lib
        VMA.lib
    )

endif()

add_library(Renderer ${RENDERER_SOURCE_FILES})
target_precompile_headers(Renderer PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)
target_include_directories(Renderer PUBLIC ${RENDERER_INCLUDE_DIRS})
target_link_directories(Renderer PUBLIC ${RENDERER_LINK_DIRS})
target_link_libraries(Renderer PUBLIC ${RENDERER_LINK_LIBS})
