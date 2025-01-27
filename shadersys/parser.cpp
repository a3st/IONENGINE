// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "parser.hpp"
#include "core/string.hpp"
#include "precompiled.h"

namespace ionengine::shadersys
{
    Parser::Parser(std::string& errors) : errors(&errors)
    {
    }

    auto Parser::parseAttributes(std::span<Token const>::iterator it) -> std::span<Token const>::iterator
    {
        return it;
    }

    auto Parser::parseData(std::span<Token const>::iterator it) -> std::span<Token const>::iterator
    {
        return it;
    }

    auto Parser::parseHeader(std::span<Token const>::iterator it) -> std::span<Token const>::iterator
    {
        auto region = it;
        it++;

        if (it->getLexeme() != Lexeme::LeftBrace)
        {
            *errors = std::format("line:{}: error {}: identifier '{}' missing end of scope", region->getNumLine(),
                                  std::to_underlying(ParseError::EOS), region->getContent());
            parseError = ParseError::EOS;
        }

        it++;

        while (it != std::span<Token const>::iterator() && it->getLexeme() != Lexeme::RightBrace)
        {
            it = parseOptionValue(it);
        }
        return it;
    }

    auto Parser::parseShaderCode(std::span<Token const>::iterator it) -> std::span<Token const>::iterator
    {
        return it;
    }

    auto Parser::parseOptionValue(std::span<Token const>::iterator it) -> std::span<Token const>::iterator
    {
        return it;
    }

    auto Parser::parseToken(std::span<Token const>::iterator it) -> std::expected<ShaderParseData, ParseError>
    {
        if (it->getLexeme() == Lexeme::Identifier)
        {
            if (it->getContent().compare("HEADER") == 0)
            {
                it = this->parseHeader(it);
            }
            else if (it->getContent().compare("DATA") == 0)
            {
                it = this->parseData(it);
            }
            else if (it->getLexeme() == Lexeme::LeftBracket)
            {
                it = this->parseAttributes(it);

                if (it->getContent().compare("VS") == 0 || it->getContent().compare("PS") == 0 ||
                    it->getContent().compare("CS") == 0)
                {
                    it = this->parseShaderCode(it);
                }
                else
                {
                    *errors = std::format("line:{}: error {}: unknown section '{}'", it->getNumLine(),
                                          std::to_underlying(ParseError::UnknownSection), it->getContent());
                    return std::unexpected(ParseError::UnknownSection);
                }
            }
            else
            {
                *errors = std::format("line:{}: error {}: unknown section '{}'", it->getNumLine(),
                                      std::to_underlying(ParseError::UnknownSection), it->getContent());
                return std::unexpected(ParseError::UnknownSection);
            }
        }

        if (parseError.has_value())
        {
            return std::unexpected(parseError.value());
        }

        if (it != std::span<Token const>::iterator())
        {
            return this->parseToken(it);
        }
        else
        {
            return std::unexpected(ParseError::EOF);
        }
    }

    auto Parser::parse(Lexer const& lexer) -> std::expected<ShaderParseData, ParseError>
    {
        auto tokens = lexer.getTokens();

        std::string materialStructureHLSL;

        auto it = tokens.begin();
        return this->parseToken(it);

        /*auto it = tokens.begin();
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
                        throw parser_error(std::format("line:{}: error {}: identifier '{}' missing end of scope",
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
                                    std::format("line:{}: error {}: identifier '{}' missing assignment (=) operator",
                                                variable->getNumLine(), std::to_underlying(ErrorCode::Operator),
                                                variable->getContent()));
                            }

                            it++;

                            if (variable->getContent().compare("Name") == 0)
                            {
                                std::string value;
                                it = this->parseOptionValue(variable, it, value);

                                headerData.name = std::move(value);
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

                                headerData.domain = std::move(value);
                            }
                            else if (variable->getContent().compare("Blend") == 0)
                            {
                                std::string value;
                                it = this->parseOptionValue(variable, it, value);

                                headerData.blend = std::move(value);
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
                        throw parser_error(std::format("line:{}: error {}: identifier '{}' missing end of scope",
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
                        throw parser_error(std::format("line:{}: error {}: identifier '{}' missing end of scope",
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
                                    std::format("line:{}: error {}: identifier '{}' missing assignment (=) operator",
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
                                    core::from_string<asset::fx::CullSide, core::serialize_oenum>(value).value_or(
                                        asset::fx::CullSide::None);
                            }
                        }
                        else
                        {
                            it++;
                        }
                    }

                    if (it == tokens.end())
                    {
                        throw parser_error(std::format("line:{}: error {}: identifier '{}' missing end of scope",
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
                        throw parser_error(std::format("line:{}: error {}: identifier '{}' missing end of scope",
                                                       region->getNumLine(), std::to_underlying(ErrorCode::EndOfScope),
                                                       region->getContent()));
                    }

                    it++;

                    if (it->getLexeme() != Lexeme::ShaderCode)
                    {
                        throw parser_error(std::format(
                            "line:{}: error {}: identifier '{}' missing shader code of any stage", region->getNumLine(),
                            std::to_underlying(ErrorCode::ShaderCode), region->getContent()));
                    }

                    stageData[core::from_string<asset::fx::StageType, core::serialize_oenum>(region->getContent())
                                  .value()] = it->getContent();

                    it++;

                    if (it->getLexeme() != Lexeme::RightBrace)
                    {
                        throw parser_error(std::format("line:{}: error {}: identifier '{}' missing end of scope",
                                                       region->getNumLine(), std::to_underlying(ErrorCode::EndOfScope),
                                                       region->getContent()));
                    }

                    it++;
                }
                else if (it->getContent().compare("DATA") == 0)
                {
                    auto region = it;

                    it++;

                    if (it->getLexeme() != Lexeme::LeftBrace)
                    {
                        throw parser_error(std::format("line:{}: error {}: identifier '{}' missing end of scope",
                                                       region->getNumLine(), std::to_underlying(ErrorCode::EndOfScope),
                                                       region->getContent()));
                    }

                    it++;

                    size_t structureSize = 0;

                    while (it != tokens.end() && it->getLexeme() != Lexeme::RightBrace)
                    {
                        if (it->getLexeme() != Lexeme::FixedType)
                        {
                            throw parser_error(std::format(
                                "line:{}: error {}: identifier '{}' missing end of scope", region->getNumLine(),
                                std::to_underlying(ErrorCode::EndOfScope), region->getContent()));
                        }

                        auto variableType = it;

                        it++;

                        if (it->getLexeme() != Lexeme::Identifier)
                        {
                            throw parser_error(std::format(
                                "line:{}: error {}: identifier '{}' missing end of scope", region->getNumLine(),
                                std::to_underlying(ErrorCode::EndOfScope), region->getContent()));
                        }

                        auto variable = it;

                        it++;

                        if (it->getLexeme() != Lexeme::Semicolon)
                        {
                            throw parser_error(std::format(
                                "line:{}: error {}: identifier '{}' missing end of scope", region->getNumLine(),
                                std::to_underlying(ErrorCode::EndOfScope), region->getContent()));
                        }

                        it++;

                        auto elementType =
                            core::from_string<asset::fx::ElementType, core::serialize_oenum>(variableType->getContent())
                                .value_or(asset::fx::ElementType::Uint);

                        asset::fx::StructureElementData elementData{.name = std::string(variable->getContent()),
                                                                    .type = elementType};
                        materialData.elements.emplace_back(std::move(elementData));

                        structureSize += asset::fx::sizeof_ElementType(elementType);

                        materialStructureHLSL +=
                            std::string(variableType->getContent()) + " " + std::string(variable->getContent()) + ";";
                    }

                    materialData.name = "MATERIAL_DATA";
                    materialData.size = structureSize;

                    if (it->getLexeme() != Lexeme::RightBrace)
                    {
                        throw parser_error(std::format("line:{}: error {}: identifier '{}' missing end of scope",
                                                       region->getNumLine(), std::to_underlying(ErrorCode::EndOfScope),
                                                       region->getContent()));
                    }

                    it++;
                }
                else
                {
                    throw parser_error(std::format("line:{}: error {}: identifier '{}' cannot be declared",
                                                   it->getNumLine(), std::to_underlying(ErrorCode::EndOfScope),
                                                   it->getContent()));
                }
            }
            else
            {
                throw parser_error(std::format("line:{}: error {}: token cannot be declared", it->getNumLine(),
                                               std::to_underlying(ErrorCode::EndOfScope)));
            }
        }

        for (auto& [stageType, shaderCode] : stageData)
        {
            shaderCode = "#include \"shared/internal.hlsli\"\nstruct MATERIAL_DATA { " + materialStructureHLSL +
                         " };\n" + shaderCode;
        }*/
    }
} // namespace ionengine::shadersys