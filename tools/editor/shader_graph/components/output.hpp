// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "graph/node.hpp"

namespace ionengine::tools::editor
{
    IONENGINE_NODE_COMPONENT_BEGIN(UnlitOutput, "Output Node", false, std::nullopt, true)
    auto setInputs() const -> std::vector<Node::SocketInfo>
    {
        return {{"Color", "float4"}};
    }

    auto generateInitialShaderCode(Node const& node) const -> std::string override
    {
        return R"(
            struct VS_INPUT {
                uint id: SV_VertexID;
            };

            struct VS_OUTPUT {
                float4 position: SV_Position;
                float2 uv : TEXCOORD0;
            };

            struct PS_OUTPUT {
                float4 color : SV_Target0;
            };
        )";
    }

    auto generateComputeShaderCode(Node const& node) const -> std::string override
    {
        return R"(
            VS_OUTPUT vs_main(VS_INPUT input) {
                VS_OUTPUT output;
                output.uv = float2((input.id << 1) & 2, input.id & 2);
                output.position = float4(output.uv * 2.0f + -1.0f, 0.0f, 1.0f);
                return output;
            }

            PS_OUTPUT ps_main(VS_OUTPUT input) {
                PS_OUTPUT output;
                output.color = ##Color##;
                return output;
            }

            technique {
                pass {
                    vertexShader = vs_main();
                    pixelShader = ps_main();
                    cullSide = "back";
                    depthWrite = false;
                    stencilWrite = false;
                }
            }
        )";
    }
    IONENGINE_NODE_COMPONENT_END
} // namespace ionengine::tools::editor