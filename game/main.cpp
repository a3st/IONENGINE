// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "engine/engine.hpp"
#include "engine/graphics/graphics.hpp"
#include "precompiled.h"
#include "game.hpp"

using namespace ionengine;

auto main(int32_t argc, char** argv) -> int32_t
{
    auto engine =
        EngineBuilder()
            .withArgs(argc, argv)
            .configureModule<Graphics>([](Graphics::ModuleOptions& moduleOptions, auto& engineEnvironment) -> void {
                moduleOptions.stagingBufferSize = 30 * 1024 * 1024;
                moduleOptions.graphicsPipeline = GraphicsPipelineBuilder()
                                                     .addSubpass(SubpassCreateInfo{.name = "S"},
                                                                 [](GraphicsContext& context) -> void {

                                                                 })
                                                     .build();
            })
            .registerModule<Game>()
            .build();

    return engine->run();
}