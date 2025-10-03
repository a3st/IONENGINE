// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "compiler.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "precompiled.h"
#include <gtest/gtest.h>

using namespace ionengine;

std::vector<std::filesystem::path> const filePaths{"shaders/ui_base.fx", "shaders/s_pbr_opaque.fx",
                                                   "shaders/pp_quad.fx"};
TEST(ShaderSystem, Lexer_Test)
{
    shadersys::Lexer lexer;

    for (auto const& filePath : filePaths)
    {
        auto p = std::filesystem::current_path();

        auto lexerResult = lexer.parse(filePath);

        if (!lexerResult.has_value())
        {
            std::cerr << lexerResult.error().what() << std::endl;
        }

        ASSERT_TRUE(lexerResult.has_value());
    }
}

TEST(ShaderSystem, Parser_Test)
{
    shadersys::Lexer lexer;
    shadersys::Parser parser;

    for (auto const& filePath : filePaths)
    {
        auto lexerResult = lexer.parse(filePath);

        ASSERT_TRUE(lexerResult.has_value());

        auto parserResult = parser.parse(lexerResult.value());

        if (!parserResult.has_value())
        {
            std::cerr << parserResult.error().what() << std::endl;
        }

        ASSERT_TRUE(parserResult.has_value());
    }
}

TEST(ShaderSystem, Compiler_DXIL_Test)
{
    auto shaderCompiler =
        shadersys::ShaderCompiler::create(asset::fx::ShaderFormat::DXIL, std::filesystem::current_path() / "include");

    for (auto const& filePath : filePaths)
    {
        auto compileResult = shaderCompiler->compile(filePath);

        if (!compileResult.has_value())
        {
            std::cerr << compileResult.error().what() << std::endl;
        }

        ASSERT_TRUE(compileResult.has_value());
    }
}

TEST(ShaderSystem, Compiler_SPIRV_Test)
{
    auto shaderCompiler =
        shadersys::ShaderCompiler::create(asset::fx::ShaderFormat::SPIRV, std::filesystem::current_path() / "include");

    for (auto const& filePath : filePaths)
    {
        auto compileResult = shaderCompiler->compile(filePath);

        if (!compileResult.has_value())
        {
            std::cerr << compileResult.error().what() << std::endl;
        }

        ASSERT_TRUE(compileResult.has_value());
    }
}

auto main(int32_t argc, char** argv) -> int32_t
{
    testing::InitGoogleTest(&argc, argv);
    return ::RUN_ALL_TESTS();
}