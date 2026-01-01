cmake_minimum_required(VERSION 3.25.1)

find_package(RmlUi CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

set(SUB_MODULE_NAME engine)

add_library(${SUB_MODULE_NAME} STATIC
    ../shadersys/fx.cpp
    ../mdl/mdl.cpp

    #gui/rml.cpp
    #gui/gui_widget.cpp
    #gui/gui.cpp

    #graphics/passes/internal/swapchain.cpp
    #graphics/passes/internal/ui.cpp
    #graphics/passes/geometry.cpp

    #graphics/image.cpp
    #graphics/surface.cpp
    #graphics/mesh.cpp
    #graphics/upload_manager.cpp
    #graphics/render_queue.cpp
    #graphics/camera.cpp
    #graphics/material.cpp
    #graphics/render_pass.cpp
    #graphics/shader.cpp
    #graphics/entity.cpp
    #graphics/light.cpp
    #graphics/world.cpp

    graphics/shader.cpp
    graphics/buffer_pool.cpp
    graphics/texture_pool.cpp
    graphics/subpass.cpp
    graphics/attachment.cpp
    graphics/graphics_pipeline.cpp
    graphics/graphics.cpp

    window/window.cpp

    logger.cpp
    engine_environment.cpp
    engine.cpp
)

target_include_directories(${SUB_MODULE_NAME} PUBLIC 
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/engine
)

target_link_libraries(${SUB_MODULE_NAME} PUBLIC 
    core
    platform
    rhi
    RmlCore
    RmlDebugger
    spdlog::spdlog
)

foreach(CONFIG_TYPE DEBUG RELEASE MINSIZEREL RELWITHDEBINFO)
    set_target_properties(${SUB_MODULE_NAME} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/lib
        LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/lib
        RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/${SUB_MODULE_NAME}/bin
    )
endforeach()

target_precompile_headers(${SUB_MODULE_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/precompiled.h)