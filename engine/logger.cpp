// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "logger.hpp"
#include "precompiled.h"
#include <spdlog/sinks/stdout_color_sinks.h>

namespace ionengine
{
    Logger::Logger(core::ref_ptr<platform::App> app, EngineEnvironment& environment, ModuleOptions const& options)
        : _environment(environment)
    {
        assert(app && "core::ref_ptr<platform::App> is nullptr");
        _app = app;

        _logger =
            std::make_unique<spdlog::logger>("IONENGINE", std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    }

    Logger::Logger(core::ref_ptr<platform::App> app, EngineEnvironment& environment) : Logger(app, environment, {})
    {
    }
} // namespace ionengine