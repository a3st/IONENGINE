cmake_minimum_required(VERSION 3.25.1)

find_package(argh CONFIG REQUIRED)

set(SUB_MODULE_NAME game)

add_executable(game 
    main.cpp

    game.cpp
)

target_include_directories(game PRIVATE 
    ${PROJECT_SOURCE_DIR}
)

target_precompile_headers(game PRIVATE ${PROJECT_SOURCE_DIR}/precompiled.h)

target_link_libraries(game PRIVATE
    engine
)

foreach(CONFIG_TYPE DEBUG RELEASE MINSIZEREL RELWITHDEBINFO)
    set_target_properties(${SUB_MODULE_NAME} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/tools/${SUB_MODULE_NAME}/lib
        LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/tools/${SUB_MODULE_NAME}/lib
        RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/tools/${SUB_MODULE_NAME}/bin
    )
endforeach()