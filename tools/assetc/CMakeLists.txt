cmake_minimum_required(VERSION 3.25.1)

find_package(argh CONFIG REQUIRED)

set(SUB_MODULE_NAME assetc)

add_executable(assetc main.cpp)

target_include_directories(assetc PRIVATE 
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/tools/assetc
)

target_precompile_headers(assetc PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

target_link_libraries(assetc PRIVATE
    argh
    mdl
    txe
)

foreach(CONFIG_TYPE DEBUG RELEASE MINSIZEREL RELWITHDEBINFO)
    set_target_properties(${SUB_MODULE_NAME} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/tools/${SUB_MODULE_NAME}/lib
        LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/tools/${SUB_MODULE_NAME}/lib
        RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/tools/${SUB_MODULE_NAME}/bin
    )
endforeach()