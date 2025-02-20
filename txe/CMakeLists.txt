cmake_minimum_required(VERSION 3.25.1)

add_library(txe STATIC
    cmp/cmp.cpp
    txe.cpp)

target_include_directories(txe PUBLIC 
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/txe
    ${PROJECT_SOURCE_DIR}/txe/thirdparty/compressonator/include)

target_link_directories(txe PUBLIC
    ${PROJECT_SOURCE_DIR}/txe/thirdparty/compressonator/lib/x64)

target_link_libraries(txe PUBLIC 
    core
    CMP_Framework_MDd)

target_precompile_headers(txe PUBLIC ${PROJECT_SOURCE_DIR}/precompiled.h)