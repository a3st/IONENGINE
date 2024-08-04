// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "tools/shaderc/lexer.hpp"
#include <gtest/gtest.h>

using namespace ionengine;
namespace shaderc = ionengine::tools::shaderc;

TEST(shaderc, Lexer_Test)
{
    std::string basicShader = R"(
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
            return output;
        }
    )";

    std::string expectedShader = R"(
        
    )";

    shaderc::Lexer lexer(basicShader);

    std::cout << lexer.getTokens().size() << std::endl;
}

auto main(int32_t argc, char** argv) -> int32_t
{
    testing::InitGoogleTest(&argc, argv);
    return ::RUN_ALL_TESTS();
}