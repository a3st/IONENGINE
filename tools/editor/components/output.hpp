// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../node.hpp"

namespace ionengine::tools::editor
{
    class OutputNode : public Node
    {
      public:
        OutputNode(uint64_t const nodeID, uint32_t const posX, uint32_t const posY)
            : Node(nodeID, "Output Image", posX, posY, {{"Color", "float4"}}, {})
        {
        }

        auto generateInitialShaderCode() -> std::string override
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

        auto generateResourceShaderCode() -> std::string override
        {
            return "";
        }

        auto generateComputeShaderCode() -> std::string override
        {
            return R"(
                PS_OUTPUT ps_main(VS_OUTPUT input) {
                    PS_OUTPUT output;
                    output.color = ##Color##;
                    return output;
                }
            )";
        }
    };
} // namespace ionengine::tools::editor