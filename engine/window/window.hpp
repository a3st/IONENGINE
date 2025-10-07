// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "engine_environment.hpp"
#include "iengine_module.hpp"
#include "platform/platform.hpp"

namespace ionengine
{
    class Window : public IEngineModule
    {
      public:
        struct ModuleOptions
        {
        };

        Window(core::ref_ptr<platform::App> app, EngineEnvironment& environment);

        Window(core::ref_ptr<platform::App> app, EngineEnvironment& environment, ModuleOptions const& options);

        auto setMouseEnabled(bool const enabled) -> void;

        auto initialize() -> void override
        {
        }

        auto shutdown() -> void override
        {
        }

        auto getPriority() const -> uint16_t override
        {
            return std::to_underlying(EngineModulePriority::Core) + 1;
        }

      private:
        EngineEnvironment& _environment;
        core::ref_ptr<platform::App> _app;
    };
} // namespace ionengine