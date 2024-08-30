// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "tools/shaderc/lexer.hpp"
#include "tools/shaderc/parser.hpp"
#include <gtest/gtest.h>

using namespace ionengine;
namespace shaderc = ionengine::tools::shaderc;

TEST(shaderc, Lexer_Test)
{
    std::string const basicShader = R"(
        import = ( 
            "engine",
            "std" 
        )

        fullscreenTexture: Texture2D constant(0);
        linearSampler: SamplerState constant(1);

        VS_INPUT: struct = {
            id: uint semantic("SV_VertexID");
        }

        VS_OUTPUT: struct = {
            position: float4 semantic("SV_Position");
            uv: float2 semantic("TEXCOORD0");
        }

        vs_main: (input: VS_INPUT) -> VS_OUTPUT = {
            output: VS_OUTPUT = (
                float4(output.uv * 2.0 + -1.0, 0.0, 1.0),
                float2((input.id << 1) & 2, input.id & 2)
            );
            return output;
        }
    )";

    shaderc::Lexer lexer(basicShader);

    ASSERT_EQ(lexer.getTokens().size(), 112);
}

TEST(shaderc, Parser_Test)
{
    std::string const basicShader = R"(
        import = ( 
            "engine",
            "std" 
        )

        fullscreenTexture: Texture2D constant(0);
        linearSampler: SamplerState constant(1);

        VS_INPUT: struct = {
            id: uint semantic("SV_VertexID");
        }

        VS_OUTPUT: struct = {
            position: float4 semantic("SV_Position");
            uv: float2 semantic("TEXCOORD0");
        }

        vs_main: (input: VS_INPUT) -> VS_OUTPUT = {
            output: VS_OUTPUT = (
                float4(output.uv * 2.0 + -1.0, 0.0, 1.0),
                float2((input.id << 1) & 2, input.id & 2)
            );
            return output;
        }
    )";

    shaderc::Lexer lexer(basicShader);
    shaderc::Parser parser(lexer);
}

auto main(int32_t argc, char** argv) -> int32_t
{
    testing::InitGoogleTest(&argc, argv);
    return ::RUN_ALL_TESTS();
}