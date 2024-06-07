// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../node.hpp"

namespace ionengine::tools::editor
{
    IONENGINE_NODE_COMPONENT_BEGIN(Split_Float4, "Split (Float4)", true, "Convert", false)
    auto setInputs() const -> std::vector<Node::SocketInfo> override
    {
        return {{"Source", "float4"}};
    }

    auto setOutputs() const -> std::vector<Node::SocketInfo> override
    {
        return {{"R", "float"}, {"G", "float"}, {"B", "float"}, {"A", "float"}};
    }

    auto generateComputeShaderCode(Node const& node) const -> std::string override
    {
        return "##Source##.##__CONNECTION_LOWER__##";
    }
    IONENGINE_NODE_COMPONENT_END

    IONENGINE_NODE_COMPONENT_BEGIN(Split_Float3, "Split (Float3)", true, "Convert", false)
    auto setInputs() const -> std::vector<Node::SocketInfo> override
    {
        return {{"Source", "float3"}};
    }

    auto setOutputs() const -> std::vector<Node::SocketInfo> override
    {
        return {{"R", "float"}, {"G", "float"}, {"B", "float"}};
    }

    auto generateComputeShaderCode(Node const& node) const -> std::string override
    {
        return "##Source##.##__CONNECTION_LOWER__##";
    }
    IONENGINE_NODE_COMPONENT_END

    IONENGINE_NODE_COMPONENT_BEGIN(Join_Float4, "Join (Float4)", true, "Convert", false)
    auto setInputs() const -> std::vector<Node::SocketInfo> override
    {
        return {{"R", "float"}, {"G", "float"}, {"B", "float"}, {"A", "float"}};
    }

    auto setOutputs() const -> std::vector<Node::SocketInfo> override
    {
        return {{"Out", "float4"}};
    }

    auto generateComputeShaderCode(Node const& node) const -> std::string override
    {
        return "float4(##R##,##G##,##B##,##A##)";
    }
    IONENGINE_NODE_COMPONENT_END
} // namespace ionengine::tools::editor