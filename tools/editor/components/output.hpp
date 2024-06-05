// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../node.hpp"

namespace ionengine::tools::editor
{
    IONENGINE_NODE_COMPONENT_BEGIN(PostProcessOutput, "Output Node", false, std::nullopt, true)
    auto setInputs() -> std::vector<Node::SocketInfo>
    {
        return {{"Color", "float4"}};
    }

    auto generateInitialShaderCode(Node const& node) -> std::string override
    {
        return R"(
                struct VS_OUTPUT {
                    float4 position : SV_Position;
                    float3 normal : NORMAL;
                    float2 uv : TEXCOORD0;
                };

                struct PS_OUTPUT {
                    float4 color : SV_Target0;
                };
            )";
    }

    auto generateResourceShaderCode(Node const& node) -> std::string override
    {
        return "";
    }

    auto generateComputeShaderCode(Node const& node) -> std::string override
    {
        return R"(
                PS_OUTPUT ps_main(VS_OUTPUT input) {
                    PS_OUTPUT output;
                    output.color = ##Color##;
                    return output;
                }
            )";
    }
    IONENGINE_NODE_COMPONENT_END
} // namespace ionengine::tools::editor