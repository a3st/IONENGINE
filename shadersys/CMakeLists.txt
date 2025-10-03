cmake_minimum_required(VERSION 3.25.1)

find_package(xxHash CONFIG REQUIRED)

set(SUB_MODULE_NAME shadersys)
set(SUB_MODULE_THIRD_PARTY_PATH ${PROJECT_SOURCE_DIR}/shadersys/thirdparty)

add_library(${SUB_MODULE_NAME} STATIC
    lexer.cpp
    parser.cpp
    fx.cpp
    compiler.cpp
    dxc/dxc.cpp
)

target_include_directories(${SUB_MODULE_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/shadersys
)

if(WIN32)
    target_include_directories(${SUB_MODULE_NAME} PUBLIC
        ${PROJECT_SOURCE_DIR}/shadersys/thirdparty/dxc/win32/include
    )

    target_link_directories(${SUB_MODULE_NAME} PUBLIC
        ${PROJECT_SOURCE_DIR}/shadersys/thirdparty/dxc/win32/lib/${TARGET_ARCH}
    )
elseif(UNIX)
    target_include_directories(${SUB_MODULE_NAME} PUBLIC
        ${PROJECT_SOURCE_DIR}/shadersys/thirdparty/dxc/linux/include
    )

    target_link_directories(${SUB_MODULE_NAME} PUBLIC
        ${PROJECT_SOURCE_DIR}/shadersys/thirdparty/dxc/linux/lib/${TARGET_ARCH}
    )
endif()

target_link_libraries(${SUB_MODULE_NAME} PUBLIC 
    xxHash::xxhash
    core
    dxgi
    dxcompiler
)

target_compile_definitions(${SUB_MODULE_NAME} PUBLIC
    IONENGINE_SHADERSYS_DXC
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
    add_executable(${SUB_MODULE_NAME}_test shadersys_test.cpp)

    target_link_libraries(${SUB_MODULE_NAME}_test PRIVATE 
        shadersys
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

    if(WIN32)
        add_custom_command(TARGET ${SUB_MODULE_NAME}_test POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${SUB_MODULE_THIRD_PARTY_PATH}/dxc/win32/bin/${TARGET_ARCH}/dxcompiler.dll ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/bin/dxcompiler.dll
            COMMAND ${CMAKE_COMMAND} -E copy ${SUB_MODULE_THIRD_PARTY_PATH}/dxc/win32/bin/${TARGET_ARCH}/dxil.dll ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/bin/dxil.dll
        )
    elseif(UNIX)
    endif()

    add_custom_command(TARGET ${SUB_MODULE_NAME}_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/engine/shaders/shared ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/bin/include/shared

        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/engine/shaders/ui_base.fx ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/bin/shaders/ui_base.fx
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/engine/shaders/s_pbr_opaque.fx ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/bin/shaders/s_pbr_opaque.fx
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/engine/shaders/pp_quad.fx ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/bin/shaders/pp_quad.fx
    )

    gtest_discover_tests(${SUB_MODULE_NAME}_test 
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/bin
    )
endif()