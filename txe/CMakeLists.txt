cmake_minimum_required(VERSION 3.25.1)

set(SUB_MODULE_NAME txe)
set(SUB_MODULE_THIRD_PARTY_PATH ${PROJECT_SOURCE_DIR}/shadersys/thirdparty)

add_library(${SUB_MODULE_NAME} STATIC
    cmp/cmp.cpp
    importer.cpp
    txe.cpp
)

target_include_directories(${SUB_MODULE_NAME} PUBLIC 
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/txe
)

if(WIN32)
    target_include_directories(${SUB_MODULE_NAME} PUBLIC
        ${PROJECT_SOURCE_DIR}/txe/thirdparty/compressonator/win32/include
    )

    target_link_directories(${SUB_MODULE_NAME} PUBLIC
        ${PROJECT_SOURCE_DIR}/txe/thirdparty/compressonator/win32/lib/${TARGET_ARCH}
    )
elseif(UNIX)
    target_include_directories(${SUB_MODULE_NAME} PUBLIC
        ${PROJECT_SOURCE_DIR}/txe/thirdparty/compressonator/linux/include
    )

    target_link_directories(${SUB_MODULE_NAME} PUBLIC
        ${PROJECT_SOURCE_DIR}/txe/thirdparty/compressonator/linux/lib/${TARGET_ARCH}
    )
endif()

target_link_libraries(${SUB_MODULE_NAME} PUBLIC 
    core
    CMP_Framework$<$<CONFIG:Debug>:d>
)

foreach(CONFIG_TYPE DEBUG RELEASE MINSIZEREL RELWITHDEBINFO)
    set_target_properties(${SUB_MODULE_NAME} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/lib
        LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/lib
        RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/bin
    )
endforeach()

target_precompile_headers(${SUB_MODULE_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/precompiled.h)

if(BUILD_TESTING)
    add_executable(${SUB_MODULE_NAME}_test txe_test.cpp)

    target_link_libraries(${SUB_MODULE_NAME}_test PRIVATE 
        txe
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
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/assets/textures/debug-empty.png ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/bin/textures/debug-empty.png
    )

    gtest_discover_tests(${SUB_MODULE_NAME}_test 
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/bin
    )
endif()