cmake_minimum_required(VERSION 3.25.1)

find_package(xxHash CONFIG REQUIRED)

set(SUB_MODULE_NAME shadersys)

add_library(${SUB_MODULE_NAME} STATIC
    lexer.cpp
    parser.cpp
    fx.cpp
    compiler.cpp
    dxc/dxc.cpp)

target_include_directories(${SUB_MODULE_NAME} PUBLIC 
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/shadersys
    ${PROJECT_SOURCE_DIR}/dxc/thirdparty/windows/include)

target_link_directories(${SUB_MODULE_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/dxc/thirdparty/windows/lib/x64)

target_link_libraries(${SUB_MODULE_NAME} PUBLIC 
    xxHash::xxhash
    core
    dxgi
    dxcompiler)

target_compile_definitions(${SUB_MODULE_NAME} PUBLIC
    IONENGINE_SHADERSYS_DXC)

foreach(CONFIG_TYPE DEBUG RELEASE MINSIZEREL RELWITHDEBINFO)
    set_target_properties(${SUB_MODULE_NAME} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/lib
        LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/lib
        RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/bin
    )
endforeach()

target_precompile_headers(${SUB_MODULE_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/precompiled.h)