cmake_minimum_required(VERSION 3.0.0)

project(IONENGINE VERSION 0.1.0)

# Global sets
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DNDEBUG")

include_directories(${PROJECT_SOURCE_DIR}/)

#[[

    Runtime-Win32 Build Target

#]]

add_executable(Runtime-Win32

    engine/main.cpp
    engine/obj_loader.cpp
    lib/thread_pool.cpp

    platform/window_win32.cpp
    platform/window_loop_win32.cpp

    renderer/backend_d3d12.cpp
    renderer/world_renderer.cpp
    #renderer/frame_graph.cpp
    renderer/color.cpp
)

target_precompile_headers(Runtime-Win32 PRIVATE precompiled.h)

# Build Output
set_target_properties(Runtime-Win32 PROPERTIES 
    OUTPUT_NAME "IONENGINE-Runtime"
    COMPILE_FLAGS "${COMPILE_FLAGS}"
)

# Include dirs
target_include_directories(Runtime-Win32 PRIVATE
    
)

# Link Libs
target_link_libraries(Runtime-Win32
    ${Runtime-Win32}
    d3d12.lib
    dxgi.lib
    dxguid.lib
)

# Link Libs
target_link_directories(Runtime-Win32 PRIVATE

)

# Preprocessor
target_compile_definitions(Runtime-Win32 PRIVATE 
    IONENGINE_PLATFORM_WIN32
)

#[[

    Runtime-Win32-VK Build Target

#]]

add_executable(Runtime-Win32VK engine/main.cpp)
target_precompile_headers(Runtime-Win32VK PRIVATE precompiled.h)

# Build Output
set_target_properties(Runtime-Win32VK PROPERTIES 
    OUTPUT_NAME "IONENGINE-Runtime-VK"
    COMPILE_FLAGS "${COMPILE_FLAGS}"
)

# Include dirs
target_include_directories(Runtime-Win32VK PRIVATE
    ${PROJECT_SOURCE_DIR}/runtime
    "C:/Program Files (x86)/Windows Kits/10/Include/10.0.18362.0/um"
    "C:/Program Files (x86)/Windows Kits/10/Include/10.0.18362.0/winrt"
    "C:/Program Files (x86)/Windows Kits/10/Include/10.0.18362.0/"
    "C:/VulkanSDK/1.2.189.2/Include"
)

# Link Libs
target_link_directories(Runtime-Win32VK PRIVATE
    "C:/VulkanSDK/1.2.189.2/Lib"
    "lgfx/build/Debug"
)

# Link Libs
target_link_libraries(Runtime-Win32VK
    ${Runtime-Win32VK}
)

# Preprocessor
target_compile_definitions(Runtime-Win32VK PRIVATE 
    IONENGINE_PLATFORM_WIN32
    IONENGINE_GFX_VK
    VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
    VK_NO_PROTOTYPES
)

#[[

    Runtime-Android Build Target

#]]

add_library(Runtime-Android SHARED engine/main.cpp)

# Build Output
set_target_properties(Runtime-Android PROPERTIES 
    OUTPUT_NAME "IONENGINE-Android"
)

# Include dirs
#target_include_directories(Runtime-Android PRIVATE
#    ${Runtime-Android}
#)

#find_library(Runtime-Android
#    log
#)

# Link Libs
#target_link_libraries(Runtime-Android
#    ${Runtime-Android}
#)

# Preprocessor
target_compile_definitions(Runtime-Android PRIVATE 
    IONENGINE_PLATFORM_ANDROID
)