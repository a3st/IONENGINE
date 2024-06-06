// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../node.hpp"

namespace ionengine::tools::editor
{
    IONENGINE_NODE_COMPONENT_BEGIN(Split_F4_F3F1, "Split (Float4 &#8594; Float3)", true, "Convert", false)
    auto setInputs() const -> std::vector<Node::SocketInfo> override
    {
        return {{"Source", "float4"}};
    }

    auto setOutputs() const -> std::vector<Node::SocketInfo> override
    {
        return {{"A", "float3"}, {"B", "float"}};
    }

    auto generateInitialShaderCode(Node const& node) const -> std::string override
    {
        return "";
    }

    auto generateResourceShaderCode(Node const& node) const -> std::string override
    {
        return "";
    }

    auto generateComputeShaderCode(Node const& node) const -> std::string override
    {
        return "";
    }
    IONENGINE_NODE_COMPONENT_END
} // namespace ionengine::tools::editor