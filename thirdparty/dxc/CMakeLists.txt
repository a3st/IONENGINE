cmake_minimum_required(VERSION 3.25.1)

add_library(dxc INTERFACE)

message(WARNING ${PROJECT_NAME})

if(WIN32)
    target_include_directories(dxc INTERFACE 
        ${CMAKE_CURRENT_SOURCE_DIR}/windows/include)

    target_link_directories(dxc INTERFACE 
        ${CMAKE_CURRENT_SOURCE_DIR}/windows/lib/x64)

    target_link_libraries(dxc INTERFACE
        dxgi 
        dxcompiler)

    add_custom_command(OUTPUT ""
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/windows/bin/x64/dxcompiler.dll ${CMAKE_BINARY_DIR}/dxcompiler.dll
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/windows/bin/x64/dxil.dll ${CMAKE_BINARY_DIR}/dxil.dll)
elseif(UNIX)
    message(FATAL_ERROR "DXC is not available on this platform")
endif()