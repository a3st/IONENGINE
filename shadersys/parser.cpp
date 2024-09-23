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
                                                       region->getFilePath(), 1000, region->getContent()));
                    }

                    it++;

                    while (it != tokens.end())
                    {
                        if (it->getLexeme() == Lexeme::Identifier)
                        {
                            auto variable = it;

                            it++;

                            if (it->getLexeme() != Lexeme::Assignment)
                            {
                                throw parser_error(
                                    std::format("{}: error {}: Identifier '{}' missing assignment (=) operator",
                                                variable->getFilePath(), 1001, variable->getContent()));
                            }

                            it++;

                            std::string value;
                            it = this->parseOptionValue(variable, it, value);

                            if (value.compare("Name") == 0)
                            {
                                headerData.shaderName = value;
                            }
                            else if (value.compare("Description") == 0)
                            {
                                headerData.description = value;
                            }
                            else if (value.compare("Domain") == 0)
                            {
                                headerData.shaderDomain = value;
                            }
                        }
                        else
                        {
                            it++;
                        }
                    }

                    if (it->getLexeme() != Lexeme::RightBrace)
                    {
                        throw parser_error(std::format("{}: error {}: Identifier '{}' missing end of scope",
                                                       region->getFilePath(), 1000, region->getContent()));
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
                                                       region->getFilePath(), 1000, region->getContent()));
                    }

                    it++;

                    while (it != tokens.end())
                    {
                        if (it->getLexeme() == Lexeme::Identifier)
                        {
                            auto variable = it;

                            it++;

                            if (it->getLexeme() != Lexeme::Assignment)
                            {
                                throw parser_error(
                                    std::format("{}: error {}: Identifier '{}' missing assignment (=) operator",
                                                variable->getFilePath(), 1001, variable->getContent()));
                            }

                            it++;

                            std::string value;
                            it = this->parseOptionValue(variable, it, value);

                            if (value.compare("DepthWrite") == 0)
                            {
                                
                            }
                            else if (value.compare("StencilWrite") == 0)
                            {
                                
                            }
                            else if (value.compare("CullSide") == 0)
                            {
                                
                            }
                        }
                        else
                        {
                            it++;
                        }
                    }

                    if (it->getLexeme() != Lexeme::RightBrace)
                    {
                        throw parser_error(std::format("{}: error {}: Identifier '{}' missing end of scope",
                                                       region->getFilePath(), 1000, region->getContent()));
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

    auto Parser::parseOptionValue(std::span<Token const>::iterator variable, std::span<Token const>::iterator it,
                                  std::string& value) -> std::span<Token const>::iterator
    {
        if (it->getLexeme() == Lexeme::FloatLiteral)
        {
            value = it->getContent();
        }
        else if (it->getLexeme() == Lexeme::StringLiteral)
        {
            value = it->getContent();
        }
        else
        {
            throw parser_error(std::format("{}: error {}: Identifier '{}' has undeclared value",
                                           variable->getFilePath(), 1002, variable->getContent()));
        }

        it++;

        if (it->getLexeme() != Lexeme::Semicolon)
        {
            throw parser_error(std::format("{}: error {}: missing ';' character", it->getFilePath(), 1003));
        }

        it++;
        return it;
    }
} // namespace ionengine::shadersys