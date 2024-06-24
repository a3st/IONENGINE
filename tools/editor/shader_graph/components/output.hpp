// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "graph/node.hpp"

namespace ionengine::tools::editor
{
    IONENGINE_NODE_COMPONENT_BEGIN(UnlitOutput, "Output Node", false, std::nullopt, true)
    auto setInputs() const -> std::vector<Node::SocketInfo> override
    {
        return {{"Color", "float3"}};
    }

    auto setOptions() const -> std::unordered_map<std::string, std::string> override
    {
        return {{"CullSide", "back"}};
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
        auto inputColor = std::find_if(
            node.inputs.begin(), node.inputs.end(), [](auto const& element) { return element.socketName.compare("Color") == 0; });

        std::stringstream stream;
        stream << R"(
            VS_OUTPUT vs_main(VS_INPUT input) {
                VS_OUTPUT output;
                output.uv = float2((input.id << 1) & 2, input.id & 2);
                output.position = float4(output.uv * 2.0 + -1.0, 0.0, 1.0);
                return output;
            }

            PS_OUTPUT ps_main(VS_OUTPUT input) {
                PS_OUTPUT output;
        )";

        if (inputColor->socketType.compare("float4") == 0)
        {
            stream << "\noutput.color = ##Color##;\n";
        }
        else
        {
            stream << "\noutput.color = float4(##Color##, 1.0);\n";
        }

        stream << R"(
                return output;
            }

            technique {
                pass {
                    vertexShader = vs_main();
                    pixelShader = ps_main();
                    cullSide = "##CullSide##";
                    depthWrite = false;
                    stencilWrite = false;
                }
            }
        )";
        return stream.str();
    }
    IONENGINE_NODE_COMPONENT_END
} // namespace ionengine::tools::editor