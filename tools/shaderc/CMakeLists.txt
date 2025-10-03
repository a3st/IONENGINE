cmake_minimum_required(VERSION 3.25.1)

find_package(argh CONFIG REQUIRED)

set(SUB_MODULE_NAME shaderc)
set(SUB_MODULE_THIRD_PARTY_PATH ${PROJECT_SOURCE_DIR}/shadersys/thirdparty)

add_executable(${SUB_MODULE_NAME} main.cpp)

target_include_directories(${SUB_MODULE_NAME} PRIVATE 
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/tools/shaderc
)

target_precompile_headers(${SUB_MODULE_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

target_link_libraries(${SUB_MODULE_NAME} PRIVATE
    argh
    shadersys
)

foreach(CONFIG_TYPE DEBUG RELEASE MINSIZEREL RELWITHDEBINFO)
    set_target_properties(${SUB_MODULE_NAME} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/tools/${SUB_MODULE_NAME}/lib
        LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/tools/${SUB_MODULE_NAME}/lib
        RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/tools/${SUB_MODULE_NAME}/bin
    )
endforeach()

if(WIN32)
    add_custom_command(TARGET ${SUB_MODULE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${SUB_MODULE_THIRD_PARTY_PATH}/dxc/win32/bin/${TARGET_ARCH}/dxcompiler.dll ${CMAKE_BINARY_DIR}/tools/${SUB_MODULE_NAME}/bin/dxcompiler.dll
        COMMAND ${CMAKE_COMMAND} -E copy ${SUB_MODULE_THIRD_PARTY_PATH}/dxc/win32/bin/${TARGET_ARCH}/dxil.dll ${CMAKE_BINARY_DIR}/tools/${SUB_MODULE_NAME}/bin/dxil.dll
    )
elseif(UNIX)
    message(FATAL_ERROR "shaderc is not available on this platform")
endif()

add_custom_command(TARGET ${SUB_MODULE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/engine/shaders/shared ${CMAKE_BINARY_DIR}/tools/${SUB_MODULE_NAME}/bin/include/shared
)