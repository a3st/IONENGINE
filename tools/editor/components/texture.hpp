// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../node.hpp"

namespace ionengine::tools::editor
{
    IONENGINE_NODE_COMPONENT_BEGIN(Sampler2D, "Sampler2D", true, "Texture", false)
    auto setInputs() const -> std::vector<Node::SocketInfo> override
    {
        return {{"UV", "float2"}, {"Texture", "Texture2D"}};
    }

    auto setOutputs() const -> std::vector<Node::SocketInfo> override
    {
        return {{"Color", "float4"}};
    }

    auto setOptions() const -> std::unordered_map<std::string, std::string> override
    {
        return {{"UV", "input.uv"}};
    }

    auto generateInitialShaderCode(Node const& node) const -> std::string override
    {
        return R"(
            [[fx::shader_constant]] SamplerState linearSampler;
        )";
    }

    auto generateComputeShaderCode(Node const& node) const -> std::string override
    {
        return "##Texture##.Sample(linearSampler, ##UV##).rgba";
    }
    IONENGINE_NODE_COMPONENT_END
} // namespace ionengine::tools::editor