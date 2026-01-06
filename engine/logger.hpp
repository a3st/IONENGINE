// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "engine_environment.hpp"
#include "iengine_module.hpp"
#include "platform/platform.hpp"
#include <spdlog/logger.h>

namespace ionengine
{
    enum class LogLevel
    {
        Debug,
        Info,
        Warning,
        Error,
        Critical
    };

    class Logger : public IEngineModule
    {
      public:
        struct ModuleOptions
        {
        };

        Logger(core::ref_ptr<platform::App> app, EngineEnvironment& environment, ModuleOptions const& options);

        Logger(core::ref_ptr<platform::App> app, EngineEnvironment& environment);

        template <typename... Args>
        auto log(LogLevel const level, std::string_view const format, Args&&... args) -> void
        {
            spdlog::level::level_enum levelEnum;
            switch (level)
            {
                case LogLevel::Debug: {
                    levelEnum = spdlog::level::level_enum::debug;
                    break;
                }
                case LogLevel::Info: {
                    levelEnum = spdlog::level::level_enum::info;
                    break;
                }
                case LogLevel::Warning: {
                    levelEnum = spdlog::level::level_enum::warn;
                    break;
                }
                case LogLevel::Error: {
                    levelEnum = spdlog::level::level_enum::err;
                    break;
                }
                case LogLevel::Critical: {
                    levelEnum = spdlog::level::level_enum::critical;
                    break;
                }
            }

            _logger->log(levelEnum, format, args...);
        }

        auto initialize() -> void override
        {
        }

        auto shutdown() -> void override
        {
        }

        auto getPriority() const -> uint16_t override
        {
            return std::to_underlying(EngineModulePriority::Core) + 0;
        }

      private:
        EngineEnvironment& _environment;
        core::ref_ptr<platform::App> _app;
        std::unique_ptr<spdlog::logger> _logger;
    };
} // namespace ionengine

#ifndef IONENGINE_LOG
#define IONENGINE_LOG(LogLevel, Format, ...) _environment.getModule<Logger>()->log(LogLevel, Format, __VA_ARGS__);
#endif // IONENGINE_LOG