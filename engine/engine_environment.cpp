// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "engine_environment.hpp"
#include "precompiled.h"

namespace ionengine
{
    auto EngineEnvironment::updateModulesOrder() -> void
    {
        _modulesOrder.clear();

        using ModuleEntry = std::pair<std::type_index, core::ref_ptr<IEngineModule>>;

        auto moduleEntries = _engineModules | std::views::transform([](auto const& x) -> ModuleEntry {
                                 return {x.first, core::ref_ptr<IEngineModule>(x.second)};
                             }) |
                             std::ranges::to<std::vector>();

        std::sort(moduleEntries.begin(), moduleEntries.end(),
                  [](auto const& a, auto const& b) { return a.second->getPriority() < b.second->getPriority(); });

        for (auto const& [moduleType, _] : moduleEntries)
        {
            _modulesOrder.emplace_back(moduleType);
        }
    }

    auto EngineEnvironment::initializeModules() -> void
    {
        for (auto const& moduleType : _modulesOrder)
        {
            assert(_engineModules[moduleType] && "core::ref_ptr<IEngineModule> is nullptr in _engineModules");
            _engineModules[moduleType]->initialize();
        }
    }

    auto EngineEnvironment::shutdownModules() -> void
    {
        for (auto const& moduleType : _modulesOrder | std::views::reverse)
        {
            assert(_engineModules[moduleType] && "core::ref_ptr<IEngineModule> is nullptr in _engineModules");
            _engineModules[moduleType]->shutdown();
        }
    }
} // namespace ionengine