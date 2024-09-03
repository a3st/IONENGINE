// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "rhi/fx/lexer.hpp"
#include "rhi/fx/parser.hpp"
#include <gtest/gtest.h>

using namespace ionengine;
namespace shaderc = ionengine::rhi::fx;

TEST(shaderc, Lexer_Test)
{
    std::string const basicShader = R"(
        Quad {
            ShaderData {
                fullscreenTexture: {
                    type: Texture2D,
                    default: "assets/debug-empty"
                },

                linearSampler: {
                    type: SamplerState
                },

                screenBuffer: {
                    type: ConstantBuffer,
                    buffer: {
                        opacity: {
                            type: float
                        },
                        alpha: {
                            type: int
                        },
                        emptyTexture: {
                            type: Texture2D
                        }
                    }
                }
            }

            Include [
                "engine.fx"
            ]

            VertexShader {
                struct VS_INPUT {
                    uint id: SV_VertexID;
                };

                struct VS_OUTPUT {
                    float4 position: SV_Position;
                    float2 uv: TEXCOORD0;
                };

                VS_OUTPUT vs_main(VS_INPUT input) {
                    VS_OUTPUT output;
                    output.position = float4(output.uv * 2.0 + -1.0, 0.0, 1.0);
                    output.uv = float2((input.id << 1) & 2, input.id & 2);
                    return output;
                }
            }

            PixelShader {
                struct VS_OUTPUT {
                    float4 position: SV_Position;
                    float2 uv: TEXCOORD0;
                };

                struct PS_OUTPUT {
                    float4 color: SV_Target0;
                };

                PS_OUTPUT vs_main(VS_OUTPUT input) {
                    Texture2D fullscreenTexture = GetTexture(fullscreenTexture);
                    SamplerState linearSampler = GetSamplerState(linearSampler);

                    VS_OUTPUT output;
                    output.color = fullscreenTexture.Sample(linearSampler, input.uv);
                    return output;
                }
            }
        }
    )";

    shaderc::Lexer lexer(basicShader, "basic.fx");

    std::cout << lexer.getTokens().size() << std::endl;

    // ASSERT_EQ(lexer.getTokens().size(), 112);
}

TEST(shaderc, Parser_Test)
{
    std::string const basicShader = R"(
        Quad {
            ShaderData {
                fullscreenTexture: {
                    type: Texture2D,
                    default: "assets/debug-empty"
                },

                linearSampler: {
                    type: SamplerState
                },

                screenBuffer: {
                    type: ConstantBuffer,
                    buffer: {
                        opacity: {
                            type: float
                        },
                        alpha: {
                            type: int
                        },
                        emptyTexture: {
                            type: Texture2D
                        }
                    }
                }
            }


        }
    )";

    // shaderc::Lexer lexer(basicShader, "basic.fx");
    // shaderc::Parser parser(lexer, std::filesystem::current_path());
}

auto main(int32_t argc, char** argv) -> int32_t
{
    testing::InitGoogleTest(&argc, argv);
    return ::RUN_ALL_TESTS();
}