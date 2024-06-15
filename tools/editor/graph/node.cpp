// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "node.hpp"
#include "precompiled.h"

namespace ionengine::tools::editor
{
    Node::Node(std::string_view const nodeName, uint32_t const componentID)
        : nodeName(nodeName), componentID(componentID)
    {
    }

    NodeComponent::NodeComponent(std::string_view const componentName, bool const contextRegister,
                                 std::optional<std::string_view> const groupName, bool const fixed)
        : componentName(componentName), contextRegister(contextRegister), groupName(groupName), fixed(fixed)
    {
    }

    auto NodeComponent::getName() const -> std::string_view
    {
        return componentName;
    }

    auto NodeComponent::getGroupName() const -> std::optional<std::string_view>
    {
        return groupName;
    }

    auto NodeComponent::isContextRegister() const -> bool
    {
        return contextRegister;
    }

    auto NodeComponent::isFixed() const -> bool
    {
        return fixed;
    }
} // namespace ionengine::tools::editor