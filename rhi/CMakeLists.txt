cmake_minimum_required(VERSION 3.25.1)

find_package(xxHash CONFIG REQUIRED)

set(SUB_MODULE_NAME rhi)
set(RHI_TARGET_BACKEND "DX12" CACHE STRING "RHI target backend")

add_library(${SUB_MODULE_NAME} STATIC rhi.cpp)

target_include_directories(${SUB_MODULE_NAME} PUBLIC 
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/rhi
)

target_link_libraries(${SUB_MODULE_NAME} PUBLIC 
    xxHash::xxhash
)

foreach(CONFIG_TYPE DEBUG RELEASE MINSIZEREL RELWITHDEBINFO)
    set_target_properties(${SUB_MODULE_NAME} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/lib
        LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/lib
        RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/bin
    )
endforeach()

target_precompile_headers(${SUB_MODULE_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/precompiled.h)

if(RHI_TARGET_BACKEND STREQUAL "DX12")
    if(WIN32)
        target_sources(${SUB_MODULE_NAME} PRIVATE dx12/dx12.cpp)
        
        find_package(d3d12-memory-allocator CONFIG REQUIRED)

        target_link_libraries(${SUB_MODULE_NAME} PUBLIC 
            unofficial::D3D12MemoryAllocator
            d3d12
            dxgi
            windowsapp
        )

        target_compile_definitions(${SUB_MODULE_NAME} PUBLIC 
            IONENGINE_PLATFORM_WIN32
            IONENGINE_RHI_DIRECTX12
        )
    else()
        message(FATAL_ERROR "DirectX12 is available only on Windows")
    endif()
elseif(RHI_TARGET_BACKEND STREQUAL "VK")
    if(WIN32)
        target_sources(${SUB_MODULE_NAME} PRIVATE vulkan/vk.cpp)

        find_package(Vulkan REQUIRED)
        find_package(VulkanMemoryAllocator CONFIG REQUIRED)

        target_link_libraries(${SUB_MODULE_NAME} PUBLIC 
            Vulkan::Vulkan 
            GPUOpen::VulkanMemoryAllocator
        )

        target_compile_definitions(${SUB_MODULE_NAME} PUBLIC 
            IONENGINE_PLATFORM_WIN32
            IONENGINE_RHI_VULKAN
        )
    elseif(UNIX)
        target_sources(${SUB_MODULE_NAME} PRIVATE vulkan/vk.cpp)

        find_package(PkgConfig REQUIRED)
        pkg_check_modules(VK REQUIRED vulkan)
        
        target_include_directories(${SUB_MODULE_NAME} PUBLIC
            ${VK_INCLUDE_DIRS}
        )
        
        target_link_directories(${SUB_MODULE_NAME} PUBLIC
            ${VK_LIBRARY_DIRS}
        )
        
        target_link_libraries(${SUB_MODULE_NAME} PUBLIC
            ${VK_LIBRARIES}
        )
        
        target_compile_definitions(${SUB_MODULE_NAME} PUBLIC
            IONENGINE_PLATFORM_X11
            IONENGINE_RHI_VULKAN
        )
    else()
        message(FATAL_ERROR "Vulkan is not available on this platform")
    endif()
endif()

if(BUILD_TESTING)
    add_executable(${SUB_MODULE_NAME}_test rhi_test.cpp)

    target_link_libraries(${SUB_MODULE_NAME}_test PRIVATE 
        rhi
        GTest::gtest
    )

    foreach(CONFIG_TYPE DEBUG RELEASE MINSIZEREL RELWITHDEBINFO)
        set_target_properties(${SUB_MODULE_NAME}_test PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/lib
            LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/lib
            RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/bin
        )
    endforeach()

    target_precompile_headers(${SUB_MODULE_NAME}_test PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

    gtest_discover_tests(${SUB_MODULE_NAME}_test 
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/bin
    )
endif()