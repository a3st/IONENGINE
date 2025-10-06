// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include "iengine_module.hpp"
#include "platform/platform.hpp"

namespace ionengine
{
    class EngineEnvironment
    {
      public:
        template <typename Type>
        auto getModule() -> core::ref_ptr<Type>
        {
            std::lock_guard lock(_mutex);

            auto moduleResult = _engineModules.find(std::type_index(typeid(Type)));
            if (moduleResult != _engineModules.end())
            {
                return moduleResult->second;
            }
            else
            {
                return nullptr;
            }
        }

        template <typename Type, typename... Args>
        auto registerModule(Args&&... args) -> bool
        {
            std::lock_guard lock(_mutex);

            auto moduleResult = _engineModules.find(std::type_index(typeid(Type)));
            if (moduleResult != _engineModules.end())
            {
                return false;
            }
            else
            {
                core::ref_ptr<IEngineModule> newModule = core::make_ref<Type>(std::forward<Args>(args)...);
                _engineModules[std::type_index(typeid(Type))] = newModule;
                return true;
            }
        }

      private:
        std::unordered_map<std::type_index, core::ref_ptr<IEngineModule>> _engineModules;
        std::mutex _mutex;
    };

    class Engine : public core::ref_counted_object
    {
      public:
        Engine();

        virtual ~Engine() = default;

        auto run() -> int32_t;

        auto getEnvironment() -> EngineEnvironment&;

      protected:
        // virtual auto onStart() -> void = 0;

        // virtual auto onUpdate(float const deltaTime) -> void = 0;

        // virtual auto onRender() -> void = 0;

      private:
        EngineEnvironment _environment;

        core::ref_ptr<platform::App> _app;
        // core::ref_ptr<rhi::RHI> RHI;

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

            return *this;
        }

      private:
    };
} // namespace ionengine