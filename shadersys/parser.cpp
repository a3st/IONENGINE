// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "parser.hpp"
#include "core/string.hpp"
#include "precompiled.h"

namespace ionengine::shadersys
{
    auto Parser::parse(Lexer const& lexer, fx::ShaderHeaderData& headerData, fx::ShaderOutputData& outputData,
                       std::unordered_map<fx::ShaderStageType, std::string>& stageData) -> bool
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
                        throw parser_error(std::format("line {}: error {}: Identifier '{}' missing end of scope",
                                                       region->getNumLine(), std::to_underlying(ErrorCode::EndOfScope),
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
                                    std::format("line {}: error {}: Identifier '{}' missing assignment (=) operator",
                                                variable->getNumLine(), std::to_underlying(ErrorCode::Operator),
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
                        throw parser_error(std::format("line {}: error {}: Identifier '{}' missing end of scope",
                                                       region->getNumLine(), std::to_underlying(ErrorCode::EndOfScope),
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
                        throw parser_error(std::format("line {}: error {}: Identifier '{}' missing end of scope",
                                                       region->getNumLine(), std::to_underlying(ErrorCode::EndOfScope),
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
                                    std::format("line {}: error {}: Identifier '{}' missing assignment (=) operator",
                                                variable->getNumLine(), std::to_underlying(ErrorCode::Operator),
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
                        throw parser_error(std::format("line {}: error {}: Identifier '{}' missing end of scope",
                                                       region->getNumLine(), std::to_underlying(ErrorCode::EndOfScope),
                                                       region->getContent()));
                    }

                    it++;
                }
                else if (it->getContent().compare("VS") == 0 || it->getContent().compare("PS") == 0 ||
                         it->getContent().compare("CS") == 0)
                {
                    auto region = it;

                    it++;

                    if (it->getLexeme() != Lexeme::LeftBrace)
                    {
                        throw parser_error(std::format("line {}: error {}: Identifier '{}' missing end of scope",
                                                       region->getNumLine(), std::to_underlying(ErrorCode::EndOfScope),
                                                       region->getContent()));
                    }

                    it++;

                    if (it->getLexeme() != Lexeme::ShaderCode)
                    {
                        throw parser_error(std::format(
                            "line {}: error {}: Identifier '{}' missing shader code of any stage", region->getNumLine(),
                            std::to_underlying(ErrorCode::ShaderCode), region->getContent()));
                    }

                    stageData[core::from_string<fx::ShaderStageType, core::serialize_oenum>(region->getContent())
                                  .value()] = it->getContent();

                    it++;

                    if (it->getLexeme() != Lexeme::RightBrace)
                    {
                        throw parser_error(std::format("line {}: error {}: Identifier '{}' missing end of scope",
                                                       region->getNumLine(), std::to_underlying(ErrorCode::EndOfScope),
                                                       region->getContent()));
                    }

                    it++;
                }
            }
            else if (it->getLexeme() == Lexeme::RightBrace)
            {
                throw parser_error(std::format("line {}: error {}: File are missing end of scope", it->getNumLine(),
                                               std::to_underlying(ErrorCode::EndOfScope)));
            }
            else
            {
                it++;
            }
        }
        return true;
    }
} // namespace ionengine::shadersys