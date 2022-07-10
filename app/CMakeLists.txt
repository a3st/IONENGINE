cmake_minimum_required(VERSION 3.0.0)

add_executable(App
    app.cpp
)

target_include_directories(App PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/modules
    ${PROJECT_SOURCE_DIR}/thirdparty/range-v3/include
)

target_link_libraries(App Platform)

target_precompile_headers(App PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)