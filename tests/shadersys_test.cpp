// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "shadersys/compiler.hpp"
#include "shadersys/lexer.hpp"
#include "shadersys/parser.hpp"
#include <gtest/gtest.h>

using namespace ionengine;

TEST(ShaderSystem, Lexer_Test)
{
    std::string const quadShader = R"(
        // Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

        HEADER {
            Name = "Quad";
            Description = "Quad Shader";
            Domain = "Screen";
        }

        DATA {
            Texture2D inputTexture;
        }

        VS {
            #include "shared/common.hlsli"

            VS_OUTPUT main(VS_INPUT input) {
                VS_OUTPUT output;
                output.position = float4(output.uv * 2.0 + -1.0, 0.0, 1.0);
                output.uv = float2((input.id << 1) & 2, input.id & 2);
                return output;
            }
        }

        PS {
            #include "shared/common.hlsli"

            PS_OUTPUT main(VS_OUTPUT input) {
                Texture2D inputTexture = GetResource(material, inputTexture);
                SamplerState linearSampler = GetSampler();

                VS_OUTPUT output;
                output.color = inputTexture.Sample(linearSampler, input.uv);
                return output;
            }
        }

        OUTPUT {
            CullSide = "back";
            DepthWrite = false;
            StencilWrite = false;
        }
    )";

    shadersys::Lexer lexer(quadShader);
}

TEST(ShaderSystem, Parser_Test)
{
    std::string const quadShader = R"(
        HEADER {
            Name = "Quad";
            Description = "Quad Shader";
            Domain = "Screen";
        }

        VS {
            #include "shared/common.hlsli"

            VS_OUTPUT main(VS_INPUT input) {
                VS_OUTPUT output;
                output.position = float4(output.uv * 2.0 + -1.0, 0.0, 1.0);
                output.uv = float2((input.id << 1) & 2, input.id & 2);
                return output;
            }
        }

        PS {
            #include "shared/common.hlsli"

            PS_OUTPUT main(VS_OUTPUT input) {
                Texture2D inputTexture = GetResource(material, inputTexture);
                SamplerState linearSampler = GetSampler();

                VS_OUTPUT output;
                output.color = inputTexture.Sample(linearSampler, input.uv);
                return output;
            }
        }

        OUTPUT {
            CullSide = "back";
            DepthWrite = false;
            StencilWrite = false;
        }
    )";

    shadersys::Lexer lexer(quadShader);
    shadersys::Parser parser;

    shadersys::fx::ShaderHeaderData headerData;
    shadersys::fx::ShaderOutputData outputData;
    std::unordered_map<shadersys::fx::ShaderStageType, std::string> stageData;
    parser.parse(lexer, headerData, outputData, stageData);
}

TEST(ShaderSystem, Compiler_Test)
{
    auto shaderCompiler = shadersys::ShaderCompiler::create(shadersys::fx::ShaderAPIType::DXIL);
    shaderCompiler->compileFromFile("../../engine/shaders/quad.fx");
}

auto main(int32_t argc, char** argv) -> int32_t
{
    testing::InitGoogleTest(&argc, argv);
    return ::RUN_ALL_TESTS();
}