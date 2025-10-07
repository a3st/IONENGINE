// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include "engine_environment.hpp"
#include "iengine_module.hpp"
#include "platform/platform.hpp"

namespace ionengine
{
    class Engine : public core::ref_counted_object
    {
      public:
        Engine();

        virtual ~Engine() = default;

        auto run() -> int32_t;

        auto getEnvironment() -> EngineEnvironment&;

        auto getApp() -> core::ref_ptr<platform::App>;

      protected:
        // virtual auto onStart() -> void = 0;

        // virtual auto onUpdate(float const deltaTime) -> void = 0;

        // virtual auto onRender() -> void = 0;

      private:
        EngineEnvironment _environment;
        core::ref_ptr<platform::App> _app;

        std::chrono::high_resolution_clock::time_point beginFrameTime;

        // std::unique_ptr<Graphics> graphics;
        // std::unique_ptr<GUI> gui;
    };

    class EngineBuilder
    {
      public:
        EngineBuilder() = default;

        auto withArgs(uint32_t argc, char** argv) -> EngineBuilder&;

        auto withAppName(std::string_view const appName) -> EngineBuilder&;

        auto build() -> core::ref_ptr<Engine>;

        template <typename Type>
        auto configureModule(std::function<void(typename Type::ModuleOptions&, EngineEnvironment&)>&& configFunction)
            -> EngineBuilder&
        {
            auto newFunction = [configFunction](core::ref_ptr<platform::App> app,
                                                EngineEnvironment& environment) -> void {
                typename Type::ModuleOptions options{};
                configFunction(options, environment);
                environment.registerModule<Type>(app, environment, options);
            };
            _configureFunctions.emplace_back(std::move(newFunction));
            return *this;
        }

      private:
        std::vector<std::function<void(core::ref_ptr<platform::App>, EngineEnvironment&)>> _configureFunctions;
    };
} // namespace ionengine