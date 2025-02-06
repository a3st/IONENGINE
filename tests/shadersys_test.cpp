// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "shadersys/compiler.hpp"
#include "shadersys/lexer.hpp"
#include "shadersys/parser.hpp"
#include <gtest/gtest.h>

using namespace ionengine;

TEST(ShaderSystem, Lexer_Test)
{
    std::string errors;
    shadersys::Lexer lexer;
    auto result = lexer.parse("../../engine/shaders/quad.fx", errors);
    ASSERT_TRUE(result.has_value());
}

TEST(ShaderSystem, Parser_Test)
{
    std::string errors;
    shadersys::Lexer lexer;
    auto lexerResult = lexer.parse("../../engine/shaders/quad.fx", errors);
    shadersys::Parser parser;
    auto parserResult = parser.parse(lexerResult.value(), errors);
    ASSERT_TRUE(parserResult.has_value());
}

TEST(ShaderSystem, Compiler_Test)
{
    auto shaderCompiler = shadersys::ShaderCompiler::create(asset::fx::ShaderFormat::DXIL);
    std::string errors;
    auto compileResult = shaderCompiler->compileFromFile("../../engine/shaders/quad.fx", errors);
    ASSERT_TRUE(compileResult.has_value());
}

auto main(int32_t argc, char** argv) -> int32_t
{
    testing::InitGoogleTest(&argc, argv);
    return ::RUN_ALL_TESTS();
}