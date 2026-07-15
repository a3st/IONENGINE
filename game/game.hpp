// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "engine/iengine_module.hpp"
#include "engine_environment.hpp"
#include "platform/platform.hpp"

namespace ionengine
{
    class Game : public IEngineModule
    {
      public:
        struct ModuleOptions
        {
        };

        Game(core::ref_ptr<platform::App> app, EngineEnvironment& environment, ModuleOptions const& options);

        Game(core::ref_ptr<platform::App> app, EngineEnvironment& environment);

        auto initialize() -> void override;

        auto shutdown() -> void override;

        auto getPriority() const -> uint16_t override
        {
            return std::to_underlying(EngineModulePriority::SubSystem) + 1;
        }
    };
} // namespace ionengine