// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "parser.hpp"
#include "core/string.hpp"
#include "precompiled.h"

namespace ionengine::shadersys
{
    auto Parser::parse(Lexer const& lexer, fx::ShaderHeaderData& headerData, fx::ShaderOutputData& outputData) -> bool
    {
        auto tokens = lexer.getTokens();

        auto it = tokens.begin();
        while (it != tokens.end())
        {
            if (it->getLexeme() == Lexeme::Identifier)
            {
                if (it->getContent().compare("HEADER") == 0)
                {
                    auto region = it;
                    it++;

                    if (it->getLexeme() != Lexeme::LeftBrace)
                    {
                        throw parser_error(std::format("{}: error {}: Identifier '{}' missing end of scope",
                                                       region->getFilePath(), std::to_underlying(ErrorCode::EndOfScope),
                                                       region->getContent()));
                    }

                    it++;

                    while (it != tokens.end() && it->getLexeme() != Lexeme::RightBrace)
                    {
                        if (it->getLexeme() == Lexeme::Identifier)
                        {
                            auto variable = it;

                            it++;

                            if (it->getLexeme() != Lexeme::Assignment)
                            {
                                throw parser_error(
                                    std::format("{}: error {}: Identifier '{}' missing assignment (=) operator",
                                                variable->getFilePath(), std::to_underlying(ErrorCode::Operator),
                                                variable->getContent()));
                            }

                            it++;

                            if (variable->getContent().compare("Name") == 0)
                            {
                                std::string value;
                                it = this->parseOptionValue(variable, it, value);

                                headerData.shaderName = std::move(value);
                            }
                            else if (variable->getContent().compare("Description") == 0)
                            {
                                std::string value;
                                it = this->parseOptionValue(variable, it, value);

                                headerData.description = std::move(value);
                            }
                            else if (variable->getContent().compare("Domain") == 0)
                            {
                                std::string value;
                                it = this->parseOptionValue(variable, it, value);

                                headerData.shaderDomain = std::move(value);
                            }
                            else
                            {
                                std::string value;
                                it = this->parseOptionValue(variable, it, value);

                                std::cout << "Unknown" << std::endl;
                            }
                        }
                        else
                        {
                            it++;
                        }
                    }

                    if (it == tokens.end())
                    {
                        throw parser_error(std::format("{}: error {}: Identifier '{}' missing end of scope",
                                                       region->getFilePath(), std::to_underlying(ErrorCode::EndOfScope),
                                                       region->getContent()));
                    }

                    it++;
                }
                else if (it->getContent().compare("OUTPUT") == 0)
                {
                    auto region = it;
                    it++;

                    if (it->getLexeme() != Lexeme::LeftBrace)
                    {
                        throw parser_error(std::format("{}: error {}: Identifier '{}' missing end of scope",
                                                       region->getFilePath(), std::to_underlying(ErrorCode::EndOfScope),
                                                       region->getContent()));
                    }

                    it++;

                    while (it != tokens.end() && it->getLexeme() != Lexeme::RightBrace)
                    {
                        if (it->getLexeme() == Lexeme::Identifier)
                        {
                            auto variable = it;

                            it++;

                            if (it->getLexeme() != Lexeme::Assignment)
                            {
                                throw parser_error(
                                    std::format("{}: error {}: Identifier '{}' missing assignment (=) operator",
                                                variable->getFilePath(), std::to_underlying(ErrorCode::Operator),
                                                variable->getContent()));
                            }

                            it++;

                            if (variable->getContent().compare("DepthWrite") == 0)
                            {
                                bool value;
                                it = this->parseOptionValue(variable, it, value);

                                outputData.depthWrite = value;
                            }
                            else if (variable->getContent().compare("StencilWrite") == 0)
                            {
                                bool value;
                                it = this->parseOptionValue(variable, it, value);

                                outputData.depthWrite = value;
                            }
                            else if (variable->getContent().compare("CullSide") == 0)
                            {
                                std::string value;
                                it = this->parseOptionValue(variable, it, value);

                                outputData.cullSide =
                                    core::from_string<fx::ShaderCullSide, core::serialize_oenum>(value).value_or(
                                        fx::ShaderCullSide::None);
                            }
                        }
                        else
                        {
                            it++;
                        }
                    }

                    if (it == tokens.end())
                    {
                        throw parser_error(std::format("{}: error {}: Identifier '{}' missing end of scope",
                                                       region->getFilePath(), std::to_underlying(ErrorCode::EndOfScope),
                                                       region->getContent()));
                    }

                    it++;
                }
                else if (it->getContent().compare("VS") == 0 || it->getContent().compare("PS") == 0 ||
                         it->getContent().compare("CS") == 0)
                {
                }
            }
            else
            {
                it++;
            }
        }
        return true;
    }

    auto Parser::parseOutputBlockExpr(std::span<Token const>::iterator it) -> std::span<Token const>::iterator
    {
        return it;
    }
} // namespace ionengine::shadersys