cmake_minimum_required(VERSION 3.0.0)

add_library(Renderer
    #backend/backend_d3d12.cpp
    #backend/d3d12x.cpp
    backend/backend_vk.cpp
)

target_include_directories(Renderer PRIVATE
    ${PROJECT_SOURCE_DIR}/modules
    ${PROJECT_SOURCE_DIR}/thirdparty/d3d12_ma/include
    ${PROJECT_SOURCE_DIR}/thirdparty/range-v3/include
    C:/VulkanSDK/1.3.204.1/Include
)

target_precompile_headers(Renderer PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

target_link_directories(Renderer PUBLIC
    ${PROJECT_SOURCE_DIR}/thirdparty/d3d12_ma/build/${CMAKE_BUILD_TYPE}
    C:/VulkanSDK/1.3.204.1/Lib
)

target_link_libraries(Renderer
    #d3d12.lib
    #dxgi.lib
    #dxguid.lib
    #dxcompiler.lib
    #D3D12MA.lib
    vulkan-1.lib
)