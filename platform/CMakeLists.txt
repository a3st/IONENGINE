cmake_minimum_required(VERSION 3.25.1)

set(SUB_MODULE_NAME platform)

add_library(${SUB_MODULE_NAME} STATIC platform.cpp)

target_include_directories(${SUB_MODULE_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR} 
    ${PROJECT_SOURCE_DIR}/platform
)

target_link_libraries(${SUB_MODULE_NAME} PUBLIC
    core
)

target_precompile_headers(${SUB_MODULE_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/precompiled.h)

foreach(CONFIG_TYPE DEBUG RELEASE MINSIZEREL RELWITHDEBINFO)
    set_target_properties(${SUB_MODULE_NAME} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/lib
        LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/lib
        RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/bin
    )
endforeach()

if(WIN32)
    target_sources(${SUB_MODULE_NAME} PRIVATE win32/win32.cpp)

    target_compile_definitions(${SUB_MODULE_NAME} PUBLIC
        IONENGINE_PLATFORM_WIN32
    )
elseif(UNIX)
    target_sources(${SUB_MODULE_NAME} PRIVATE x11/x11.cpp)

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(X11 REQUIRED x11)

    target_include_directories(${SUB_MODULE_NAME} PUBLIC
        ${X11_INCLUDE_DIRS}
    )

    target_link_directories(${SUB_MODULE_NAME} PUBLIC
        ${X11_LIBRARY_DIRS}
    )

    target_link_libraries(${SUB_MODULE_NAME} PUBLIC
        ${X11_LIBRARIES}
    )

    target_compile_definitions(${SUB_MODULE_NAME} PUBLIC
        IONENGINE_PLATFORM_X11
    )
else()
    message(FATAL_ERROR "Unsupported target platform")
endif()

if(BUILD_TESTING)
    add_executable(${SUB_MODULE_NAME}_test platform_test.cpp)

    target_link_libraries(${SUB_MODULE_NAME}_test PRIVATE 
        GTest::gtest
        platform
    )

    foreach(CONFIG_TYPE DEBUG RELEASE MINSIZEREL RELWITHDEBINFO)
        set_target_properties(${SUB_MODULE_NAME}_test PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/lib
            LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/lib
            RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/bin
        )
    endforeach()

    target_precompile_headers(${SUB_MODULE_NAME}_test PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

    gtest_discover_tests(${SUB_MODULE_NAME}_test 
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/bin
    )
endif()