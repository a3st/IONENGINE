// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "shadersys/compiler.hpp"
#include "shadersys/lexer.hpp"
#include "shadersys/parser.hpp"
#include <gtest/gtest.h>

using namespace ionengine;

TEST(ShaderSystem, Lexer_Test)
{
    shadersys::Lexer lexer;
    auto lexerResult = lexer.parse("ui_base.fx");

    ASSERT_TRUE(lexerResult.has_value());
}

TEST(ShaderSystem, Parser_Test)
{
    shadersys::Lexer lexer;
    auto lexerResult = lexer.parse("ui_base.fx");

    shadersys::Parser parser;
    auto parserResult = parser.parse(lexerResult.value());

    if (!parserResult.has_value())
    {
        std::cerr << parserResult.error().what() << std::endl;
    }

    ASSERT_TRUE(parserResult.has_value());
}

TEST(ShaderSystem, Compiler_DXIL_Test)
{
    auto shaderCompiler =
        shadersys::ShaderCompiler::create(asset::fx::ShaderFormat::DXIL, std::filesystem::current_path() / "include");
    auto compileResult = shaderCompiler->compile("ui_base.fx");

    if (!compileResult.has_value())
    {
        std::cerr << compileResult.error().what() << std::endl;
    }

    ASSERT_TRUE(compileResult.has_value());
}

TEST(ShaderSystem, Compiler_SPIRV_Test)
{
    auto shaderCompiler =
        shadersys::ShaderCompiler::create(asset::fx::ShaderFormat::SPIRV, std::filesystem::current_path() / "include");
    auto compileResult = shaderCompiler->compile("ui_base.fx");

    if (!compileResult.has_value())
    {
        std::cerr << compileResult.error().what() << std::endl;
    }

    ASSERT_TRUE(compileResult.has_value());
}

auto main(int32_t argc, char** argv) -> int32_t
{
    testing::InitGoogleTest(&argc, argv);
    return ::RUN_ALL_TESTS();
}