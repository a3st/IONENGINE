// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include "iengine_module.hpp"

namespace ionengine
{
    class EngineEnvironment
    {
      public:
        EngineEnvironment() = default;

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
        auto registerModule(Args&&... args) -> void
        {
            std::lock_guard lock(_mutex);

            auto moduleResult = _engineModules.find(std::type_index(typeid(Type)));
            if (moduleResult == _engineModules.end())
            {
                core::ref_ptr<IEngineModule> newModule = core::make_ref<Type>(std::forward<Args>(args)...);
                _engineModules[std::type_index(typeid(Type))] = newModule;
            }

            this->updateModulesOrder();
        }

        auto initializeModules() -> void;

        auto shutdownModules() -> void;

      private:
        std::vector<std::type_index> _modulesOrder;
        std::unordered_map<std::type_index, core::ref_ptr<IEngineModule>> _engineModules;
        std::mutex _mutex;

        auto updateModulesOrder() -> void;
    };
}