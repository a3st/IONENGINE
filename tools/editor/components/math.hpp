// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../node.hpp"

namespace ionengine::tools::editor
{
    IONENGINE_NODE_COMPONENT_BEGIN(Constant_Color, "Constant (Color)", true, "Math", false)
    auto setInputs() const -> std::vector<Node::SocketInfo> override
    {
        return {{"Value", "Color"}};
    }

    auto setOutputs() const -> std::vector<Node::SocketInfo> override
    {
        return {{"Out", "float3"}};
    }

    auto setOptions() const -> std::unordered_map<std::string, std::string> override
    {
        return {{"Value", "0.0, 0.0, 0.0"}};
    }

    auto generateComputeShaderCode(Node const& node) const -> std::string override
    {
        return "float3(##Value##)";
    }
    IONENGINE_NODE_COMPONENT_END

    IONENGINE_NODE_COMPONENT_BEGIN(Constant_Float, "Constant (Float)", true, "Math", false)
    auto setInputs() const -> std::vector<Node::SocketInfo> override
    {
        return {{"Value", "Number"}};
    }

    auto setOutputs() const -> std::vector<Node::SocketInfo> override
    {
        return {{"Out", "float"}};
    }

    auto setOptions() const -> std::unordered_map<std::string, std::string> override
    {
        return {{"Value", "0.0"}};
    }

    auto generateComputeShaderCode(Node const& node) const -> std::string override
    {
        return "float(##Value##)";
    }
    IONENGINE_NODE_COMPONENT_END
} // namespace ionengine::tools::editor