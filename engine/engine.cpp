// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "engine.hpp"
#include "graphics/graphics.hpp"
#include "logger.hpp"
#include "precompiled.h"
#include "window/window.hpp"

namespace ionengine
{
    auto EngineBuilder::withArgs(uint32_t argc, char** argv) -> EngineBuilder&
    {
        return *this;
    }

    auto EngineBuilder::withAppName(std::string_view const appName) -> EngineBuilder&
    {
        return *this;
    }

    auto EngineBuilder::build() -> core::ref_ptr<Engine>
    {
        auto engine = core::make_ref<Engine>();

        for (auto const& configureFunction : _configureFunctions)
        {
            configureFunction(engine->getApp(), engine->getEnvironment());
        }

        // Try always to register standard modules
        engine->getEnvironment().registerModule<Logger>(engine->getApp(), engine->getEnvironment());
        engine->getEnvironment().registerModule<Window>(engine->getApp(), engine->getEnvironment());
        engine->getEnvironment().registerModule<Graphics>(engine->getApp(), engine->getEnvironment());

        return engine;
    }

    auto Engine::getEnvironment() -> EngineEnvironment&
    {
        return _environment;
    }

    auto Engine::getApp() -> core::ref_ptr<platform::App>
    {
        return _app;
    }

    Engine::Engine()
    {
        _app = platform::App::create("Test");

        /*

        graphics = std::make_unique<Graphics>(RHI, rhiCreateInfo.numBuffering);
        gui = std::make_unique<GUI>(RHI);

        application->windowStateChanged += [this](platform::WindowEvent const& event) -> void {
            switch (event.type)
            {
                case platform::WindowEventType::Resize: {
                    uint32_t const width = event.size.width;
                    uint32_t const height = event.size.height;

                    if (graphics && width > 0 && height > 0)
                    {
                        graphics->onResize(width, height);
                    }

                    if (gui && width > 0 && height > 0)
                    {
                        gui->onResize(width, height);
                    }
                    break;
                }
            }
        };

        application->windowUpdated += [this]() -> void {
            auto endFrameTime = std::chrono::high_resolution_clock::now();
            float deltaTime =
                std::chrono::duration_cast<std::chrono::microseconds>(endFrameTime - beginFrameTime).count() /
                1000000.0f;

            gui->onUpdate();
            this->onUpdate(deltaTime);

            beginFrameTime = std::chrono::high_resolution_clock::now();

            graphics->beginFrame();
            this->onRender();
            gui->onRender();
            graphics->endFrame();
        };*/
    }

    auto Engine::run() -> int32_t
    {
        _environment.initializeModules();

        /*this->onStart();

        beginFrameTime = std::chrono::high_resolution_clock::now();

        Material::baseSurfaceMaterial = nullptr;*/

        _app->run();

        _environment.shutdownModules();
        return EXIT_SUCCESS;
    }
} // namespace ionengine