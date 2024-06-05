// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../node.hpp"

namespace ionengine::tools::editor
{
    IONENGINE_NODE_COMPONENT_BEGIN(Sampler2D, "Sampler2D", true, "Texture", false)
    auto setInputs() -> std::vector<Node::SocketInfo> override
    {
        return {{"UV", "float2"}, {"Texture", "Texture2D"}};
    }

    auto setOutputs() -> std::vector<Node::SocketInfo> override
    {
        return {{"Color", "float4"}};
    }

    auto generateInitialShaderCode(Node const& node) -> std::string override
    {
        return R"(
                [fx::shader_constant]
                SamplerState linearSampler;
            )";
    }

    auto generateResourceShaderCode(Node const& node) -> std::string override
    {
        return "";
    }

    auto generateComputeShaderCode(Node const& node) -> std::string override
    {
        return "##Texture##.Sample(linearSampler, ##UV##).rgba";
    }
    IONENGINE_NODE_COMPONENT_END
} // namespace ionengine::tools::editor