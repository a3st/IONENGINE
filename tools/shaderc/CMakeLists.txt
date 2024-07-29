cmake_minimum_required(VERSION 3.25.1)

add_executable(shaderc
    ../../rhi/fx/fx.cpp
    fx.cpp
    main.cpp)

target_include_directories(shaderc PRIVATE 
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/tools/shaderc)

target_precompile_headers(shaderc PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

find_package(simdjson CONFIG REQUIRED)
find_package(argh CONFIG REQUIRED)

target_link_libraries(shaderc PRIVATE
    simdjson::simdjson
    argh)

set_target_properties(shaderc PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tools
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tools
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tools)

if(WIN32)
    target_include_directories(shaderc PRIVATE 
        ${PROJECT_SOURCE_DIR}/thirdparty/dxc/windows/include)

    target_link_directories(shaderc PRIVATE 
        ${PROJECT_SOURCE_DIR}/thirdparty/dxc/windows/lib/x64)

    target_link_libraries(shaderc PRIVATE
        dxgi 
        dxcompiler)

    add_custom_command(TARGET shaderc POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/thirdparty/dxc/windows/bin/x64/dxcompiler.dll ${CMAKE_BINARY_DIR}/tools/dxcompiler.dll
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/thirdparty/dxc/windows/bin/x64/dxil.dll ${CMAKE_BINARY_DIR}/tools/dxil.dll)
elseif(UNIX)
    message(FATAL_ERROR "shaderc is not available on this platform")
endif()