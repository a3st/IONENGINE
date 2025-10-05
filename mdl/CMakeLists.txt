cmake_minimum_required(VERSION 3.25.1)

find_package(tinyobjloader CONFIG REQUIRED)

set(SUB_MODULE_NAME mdl)

add_library(${SUB_MODULE_NAME} STATIC
    obj/obj.cpp
    importer.cpp
    mdl.cpp
)

target_include_directories(${SUB_MODULE_NAME} PUBLIC 
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/mdl
)

target_link_libraries(${SUB_MODULE_NAME} PUBLIC 
    core
    tinyobjloader::tinyobjloader
)

target_precompile_headers(${SUB_MODULE_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/precompiled.h)

if(BUILD_TESTING)
    add_executable(${SUB_MODULE_NAME}_test mdl_test.cpp)

    target_link_libraries(${SUB_MODULE_NAME}_test PRIVATE 
        mdl
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

    add_custom_command(TARGET ${SUB_MODULE_NAME}_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/assets/objects/box.obj ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/bin/objects/box.obj
    )

    gtest_discover_tests(${SUB_MODULE_NAME}_test 
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/bin
    )
endif()