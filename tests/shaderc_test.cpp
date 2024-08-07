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
            "stdlib" 
        )

        fullscreenTexture: Texture2D fx::constant(0);
        linearSampler: SamplerState fx::constant(1);

        VS_INPUT: struct = {
            id: uint SV_VertexID;
        }

        VS_OUTPUT: struct = {
            position: float4 SV_Position;
            uv: float2 TEXCOORD0;
        }

        vs_main: (input: VS_INPUT) -> VS_OUTPUT = {
            output: VS_OUTPUT = (
                float2((input.id << 1) & 2, input.id & 2),
                float4(output.uv * 2.0 + -1.0, 0.0, 1.0)
            );
            return output;
        }
    )";

    shaderc::Lexer lexer(basicShader);

    ASSERT_EQ(lexer.getTokens().size(), 114);
}

TEST(shaderc, Parser_Test)
{
    std::string const basicShader = R"(
        import = ( 
            "engine",
            "stdlib" 
        )

        fullscreenTexture: Texture2D fx::constant(0);
        linearSampler: SamplerState fx::constant(1);

        VS_INPUT: struct = {
            id: uint SV_VertexID;
        }

        VS_OUTPUT: struct = {
            position: float4 SV_Position;
            uv: float2 TEXCOORD0;
        }

        vs_main: (input: VS_INPUT) -> VS_OUTPUT = {
            output: VS_OUTPUT = (
                float2((input.id << 1) & 2, input.id & 2),
                float4(output.uv * 2.0 + -1.0, 0.0, 1.0)
            );
            return output;
        }
    )";

    shaderc::Lexer lexer(basicShader);

    shaderc::Parser parser(lexer.getTokens());

    
}

auto main(int32_t argc, char** argv) -> int32_t
{
    testing::InitGoogleTest(&argc, argv);
    return ::RUN_ALL_TESTS();
}