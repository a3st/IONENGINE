// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "graph/registry.hpp"
#include "core/exception.hpp"
#include "precompiled.h"

namespace ionengine::tools::editor
{
    auto ComponentRegistry::addComponentToRegistry(core::ref_ptr<NodeComponent> component) -> void
    {
        if (components.find(component->componentID) != components.end())
        {
            throw core::Exception("Component cannot be registered multiple times");
        }
        components.emplace(component->componentID, component);
    }

    auto ComponentRegistry::getComponents() const -> std::unordered_map<uint32_t, core::ref_ptr<NodeComponent>> const&
    {
        return components;
    }

    auto ComponentRegistry::getComponentFromRegistryByType(uint32_t const componentID) -> core::ref_ptr<NodeComponent>
    {
        auto found = components.find(componentID);
        if (found != components.end())
        {
            return found->second;
        }
        else
        {
            return nullptr;
        }
    }
} // namespace ionengine::tools::editor