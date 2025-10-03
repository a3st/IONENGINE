cmake_minimum_required(VERSION 3.25.1)

find_package(simdjson CONFIG REQUIRED)

set(SUB_MODULE_NAME core)

add_library(${SUB_MODULE_NAME} STATIC 
    string_utils.cpp
    base64.cpp
    crc32.cpp
    color.cpp)

target_include_directories(${SUB_MODULE_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(${SUB_MODULE_NAME} PUBLIC simdjson::simdjson)

foreach(CONFIG_TYPE DEBUG RELEASE MINSIZEREL RELWITHDEBINFO)
    set_target_properties(${SUB_MODULE_NAME} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/lib
        LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/lib
        RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/bin
    )
endforeach()

target_precompile_headers(${SUB_MODULE_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/precompiled.h)