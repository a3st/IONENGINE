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
        // Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

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

    asset::fx::HeaderData headerData;
    asset::fx::OutputData outputData;
    asset::fx::StructureData materialData;
    std::unordered_map<asset::fx::StageType, std::string> stageData;
    parser.parse(lexer, headerData, outputData, stageData, materialData);
}

TEST(ShaderSystem, Compiler_Test)
{
    auto shaderCompiler = shadersys::ShaderCompiler::create(asset::fx::APIType::DXIL);
    std::string errors;
    auto shaderFile = shaderCompiler->compileFromFile("../../engine/shaders/quad.fx", errors);
    std::cout << errors << std::endl;
    core::to_file<asset::ShaderFile, core::serialize_oarchive>(shaderFile.value(), "test.bin");
}

TEST(ShaderSystem, Permutation_Test)
{
    std::vector<std::string> permutations = {"BASE", "FEATURE_SKINNING", "FEATURE_MSAA"};
    std::unordered_map<std::string, uint32_t> flags = {
        {"BASE", 1 << 0}, {"FEATURE_SKINNING", 1 << 1}, {"FEATURE_MSAA", 1 << 2}};

    auto result = shadersys::getAllVariants(permutations, flags);
    ASSERT_EQ(result[0], 1);
    ASSERT_EQ(result[1], 3);
    ASSERT_EQ(result[2], 5);
    ASSERT_EQ(result[3], 7);
}

auto main(int32_t argc, char** argv) -> int32_t
{
    testing::InitGoogleTest(&argc, argv);
    return ::RUN_ALL_TESTS();
}