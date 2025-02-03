// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "parser.hpp"
#include "core/string.hpp"
#include "precompiled.h"

namespace ionengine::shadersys
{
    Parser::Parser(std::string& errors) : errors(&errors)
    {
    }

    auto Parser::parseAttributes(std::span<Token const>::iterator it, std::optional<ParseError>& errorCode,
                                 ShaderParseData& parseData) -> std::span<Token const>::iterator
    {
        it++;

        std::unordered_map<std::string, std::string> attributes;

        while (it != tokens.end() && it->getLexeme() != Lexeme::RightBracket)
        {
            auto attribute = it;

            if (it->getLexeme() != Lexeme::Identifier)
            {
                *errors =
                    std::format("line:{}: error {}: identifier '{}' missing end of scope", attribute->getNumLine(),
                                std::to_underlying(ParseError::EOS), attribute->getContent());
                errorCode = ParseError::EOS;
            }

            it++;

            if (it->getLexeme() != Lexeme::LeftParen)
            {
                *errors = std::format("line:{}: error {}: identifier '{}' missing missing opening ('(') character",
                                      attribute->getNumLine(), std::to_underlying(ParseError::Character),
                                      attribute->getContent());
                errorCode = ParseError::Character;
            }

            it++;

            if (it->getLexeme() != Lexeme::FloatLiteral && it->getLexeme() != Lexeme::BoolLiteral &&
                it->getLexeme() != Lexeme::StringLiteral)
            {
                *errors =
                    std::format("line:{}: error {}: identifier '{}' has invalid value type", attribute->getNumLine(),
                                std::to_underlying(ParseError::InvalidType), attribute->getContent());
                errorCode = ParseError::InvalidType;
            }

            it++;

            attributes[std::string(attribute->getContent())] = std::string(it->getContent());

            if (it->getLexeme() != Lexeme::RightParen)
            {
                *errors = std::format("line:{}: error {}: identifier '{}' missing missing closing (')') character",
                                      attribute->getNumLine(), std::to_underlying(ParseError::Character),
                                      attribute->getContent());
                errorCode = ParseError::Character;
            }

            it++;

            // TODO!
        }

        auto retIt = it;
        it++;

        if (it->getContent().compare("VS") == 0)
        {
        }
        else if (it->getContent().compare("PS") == 0)
        {
            parseData.psAttributes = std::move(attributes);
        }
        else if (it->getContent().compare("CS") == 0)
        {
            parseData.csAttributes = std::move(attributes);
        }
        return ++retIt;
    }

    auto Parser::parseDataGroup(std::span<Token const>::iterator it, std::optional<ParseError>& errorCode,
                                ShaderParseData& parseData) -> std::span<Token const>::iterator
    {
        auto region = it;
        it++;

        if (it->getLexeme() != Lexeme::LeftBrace)
        {
            *errors = std::format("line:{}: error {}: identifier '{}' missing end of scope", region->getNumLine(),
                                  std::to_underlying(ParseError::EOS), region->getContent());
            errorCode = ParseError::EOS;
        }

        it++;

        while (it != tokens.end() && it->getLexeme() != Lexeme::RightBrace)
        {
            asset::fx::StructureElementData outVariable;
            it = parseStructVariable(it, errorCode, outVariable);
            parseData.materialData.size += asset::fx::sizeof_ElementType(outVariable.type);
            parseData.materialData.elements.emplace_back(outVariable);
        }

        parseData.materialData.name = "MATERIAL_DATA";
        return ++it;
    }

    auto Parser::parseStructVariable(std::span<Token const>::iterator it, std::optional<ParseError>& errorCode,
                                     asset::fx::StructureElementData& outVariable) -> std::span<Token const>::iterator
    {
        if (it->getLexeme() != Lexeme::FixedType)
        {
            *errors = std::format("line:{}: error {}: invalid variable type '{}'", it->getNumLine(),
                                  std::to_underlying(ParseError::EOS), it->getContent());
            errorCode = ParseError::InvalidType;
        }

        if (it->getContent().compare("float") == 0)
        {
            outVariable.type = asset::fx::ElementType::Float3;
        }
        else if (it->getContent().compare("float2") == 0)
        {
            outVariable.type = asset::fx::ElementType::Float2;
        }
        else if (it->getContent().compare("float3") == 0)
        {
            outVariable.type = asset::fx::ElementType::Float3;
        }
        else if (it->getContent().compare("float3x3") == 0)
        {
            outVariable.type = asset::fx::ElementType::Float3x3;
        }
        else if (it->getContent().compare("float4") == 0)
        {
            outVariable.type = asset::fx::ElementType::Float4;
        }
        else if (it->getContent().compare("float4x4") == 0)
        {
            outVariable.type = asset::fx::ElementType::Float4x4;
        }
        else if (it->getContent().compare("bool") == 0)
        {
            outVariable.type = asset::fx::ElementType::Bool;
        }
        else if (it->getContent().compare("int") == 0)
        {
            outVariable.type = asset::fx::ElementType::Sint;
        }
        else
        {
            outVariable.type = asset::fx::ElementType::Uint;
        }

        it++;

        if (it->getLexeme() != Lexeme::Identifier)
        {
            *errors = std::format("line:{}: error {}: identifier '{}' missing end of scope", it->getNumLine(),
                                  std::to_underlying(ParseError::EOS), it->getContent());
            errorCode = ParseError::EOS;
        }

        auto variable = it;

        outVariable.name = std::string(it->getContent());

        it++;

        if (it->getLexeme() != Lexeme::Semicolon)
        {
            *errors =
                std::format("line:{}: error {}: identifier '{}' missing ending (;) character", variable->getNumLine(),
                            std::to_underlying(ParseError::Character), variable->getContent());
            errorCode = ParseError::Character;
        }

        return ++it;
    }

    auto Parser::parseHeaderGroup(std::span<Token const>::iterator it, std::optional<ParseError>& errorCode,
                                  ShaderParseData& parseData) -> std::span<Token const>::iterator
    {
        auto region = it;
        it++;

        if (it->getLexeme() != Lexeme::LeftBrace)
        {
            *errors = std::format("line:{}: error {}: identifier '{}' missing end of scope", region->getNumLine(),
                                  std::to_underlying(ParseError::EOS), region->getContent());
            errorCode = ParseError::EOS;
        }

        it++;

        while (it != tokens.end() && it->getLexeme() != Lexeme::RightBrace)
        {
            std::string outVariable, outValue;
            it = parseOptionValue(it, errorCode, outVariable, outValue);

            if (outVariable.compare("Name") == 0)
            {
                parseData.headerData.name = outValue;
            }
            else if (outVariable.compare("Description") == 0)
            {
                parseData.headerData.description = outValue;
            }
            else if (outVariable.compare("Domain") == 0)
            {
                parseData.headerData.domain = outValue;
            }
        }
        return ++it;
    }

    auto Parser::parseShaderCode(std::span<Token const>::iterator it, std::optional<ParseError>& errorCode,
                                 ShaderParseData& parseData) -> std::span<Token const>::iterator
    {
        auto region = it;

        asset::fx::StageType stageType;
        if (region->getContent().compare("VS") == 0)
        {
            stageType = asset::fx::StageType::Vertex;
        }
        else if (region->getContent().compare("PS") == 0)
        {
            stageType = asset::fx::StageType::Pixel;
        }
        else if (region->getContent().compare("CS") == 0)
        {
            stageType = asset::fx::StageType::Compute;
        }

        it++;

        if (it->getLexeme() != Lexeme::LeftBrace)
        {
            *errors = std::format("line:{}: error {}: identifier '{}' missing end of scope", region->getNumLine(),
                                  std::to_underlying(ParseError::EOS), region->getContent());
            errorCode = ParseError::EOS;
        }

        it++;

        if (it->getLexeme() != Lexeme::ShaderCode)
        {
            *errors = std::format("line:{}: error {}: identifier '{}' missing end of scope", region->getNumLine(),
                                  std::to_underlying(ParseError::EOS), region->getContent());
            errorCode = ParseError::EOS;
        }

        parseData.codeData[stageType] = std::string(it->getContent());

        it++;

        if (it->getLexeme() != Lexeme::RightBrace)
        {
            *errors = std::format("line:{}: error {}: identifier '{}' missing end of scope", region->getNumLine(),
                                  std::to_underlying(ParseError::EOS), region->getContent());
            errorCode = ParseError::EOS;
        }

        return ++it;
    }

    auto Parser::parseOptionValue(std::span<Token const>::iterator it, std::optional<ParseError>& errorCode,
                                  std::string& outName, std::string& outValue) -> std::span<Token const>::iterator
    {
        if (it->getLexeme() != Lexeme::Identifier)
        {
            *errors = std::format("line:{}: error {}: identifier '{}' missing end of scope", it->getNumLine(),
                                  std::to_underlying(ParseError::EOS), it->getContent());
            errorCode = ParseError::EOS;
        }

        auto variable = it;
        it++;

        if (it->getLexeme() != Lexeme::Assignment)
        {
            *errors =
                std::format("line:{}: error {}: identifier '{}' missing assignment (=) operator",
                            variable->getNumLine(), std::to_underlying(ParseError::Operator), variable->getContent());
            errorCode = ParseError::Operator;
        }

        it++;

        if (it->getLexeme() != Lexeme::FloatLiteral && it->getLexeme() != Lexeme::BoolLiteral &&
            it->getLexeme() != Lexeme::StringLiteral)
        {
            *errors = std::format("line:{}: error {}: identifier '{}' has invalid value type", variable->getNumLine(),
                                  std::to_underlying(ParseError::InvalidType), variable->getContent());
            errorCode = ParseError::InvalidType;
        }

        outName = std::string(variable->getContent());
        outValue = std::string(it->getContent());

        it++;

        if (it->getLexeme() != Lexeme::Semicolon)
        {
            *errors =
                std::format("line:{}: error {}: identifier '{}' missing ending (;) character", variable->getNumLine(),
                            std::to_underlying(ParseError::Character), variable->getContent());
            errorCode = ParseError::Character;
        }

        return ++it;
    }

    auto Parser::parseToken(std::span<Token const>::iterator it, ShaderParseData& parseData)
        -> std::optional<ParseError>
    {
        std::optional<ParseError> errorCode;

        if (it->getLexeme() == Lexeme::Identifier)
        {
            if (it->getContent().compare("HEADER") == 0)
            {
                it = this->parseHeaderGroup(it, errorCode, parseData);
            }
            else if (it->getContent().compare("DATA") == 0)
            {
                it = this->parseDataGroup(it, errorCode, parseData);
            }
            else if (it->getLexeme() == Lexeme::LeftBracket)
            {
                it = this->parseAttributes(it, errorCode, parseData);
            }
            else if (it->getContent().compare("VS") == 0 || it->getContent().compare("PS") == 0 ||
                     it->getContent().compare("CS") == 0)
            {
                it = this->parseShaderCode(it, errorCode, parseData);
            }
            else
            {
                *errors = std::format("line:{}: error {}: unknown section '{}'", it->getNumLine(),
                                      std::to_underlying(ParseError::UnknownSection), it->getContent());
                return ParseError::UnknownSection;
            }
        }
        else
        {
            *errors = std::format("line:{}: error {}: expression is incomplete", it->getNumLine(),
                                  std::to_underlying(ParseError::EOF));
            return ParseError::EOF;
        }

        if (errorCode.has_value())
        {
            return errorCode.value();
        }

        if (it != tokens.end())
        {
            return this->parseToken(it, parseData);
        }
        else
        {
            return std::nullopt;
        }
    }

    auto Parser::parse(Lexer const& lexer) -> std::expected<ShaderParseData, ParseError>
    {
        tokens = lexer.getTokens();
        ShaderParseData parseData{};
        auto errorCode = this->parseToken(tokens.begin(), parseData);
        return errorCode.has_value() ? std::unexpected(errorCode.value())
                                     : std::expected<ShaderParseData, ParseError>(parseData);
    }
} // namespace ionengine::shadersys