// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "parser.hpp"
#include "core/string.hpp"
#include "precompiled.h"

namespace ionengine::shadersys
{
    auto Parser::parse(Lexer const& lexer, asset::fx::HeaderData& headerData, asset::fx::OutputData& outputData,
                       std::unordered_map<asset::fx::StageType, std::string>& stageData,
                       asset::fx::StructureData& materialData) -> void
    {
        auto tokens = lexer.getTokens();

        std::string materialStructureHLSL;

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
        }
    }
} // namespace ionengine::shadersys