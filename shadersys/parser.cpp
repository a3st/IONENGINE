// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "parser.hpp"
#include "core/string_utils.hpp"
#include "precompiled.h"

namespace ionengine::shadersys
{
    auto Parser::parseAttributes(std::span<Token const>::iterator it, bool& successful, ShaderParseData& output)
        -> std::span<Token const>::iterator
    {
        it++;

        std::unordered_map<std::string, std::string> attributes;

        while (it != tokens.end() && it->getLexeme() != Lexeme::RightBracket)
        {
            auto attribute = it;

            if (it->getLexeme() != Lexeme::Identifier)
            {
                errors = std::format("line:{}: error: identifier '{}' missing end of scope", attribute->getNumLine(),
                                     attribute->getContent());
                successful = false;
            }

            it++;

            if (it->getLexeme() != Lexeme::LeftParen)
            {
                errors = std::format("line:{}: error: identifier '{}' missing missing opening ('(') character",
                                     attribute->getNumLine(), attribute->getContent());
                successful = false;
            }

            it++;

            if (it->getLexeme() != Lexeme::FloatLiteral && it->getLexeme() != Lexeme::BoolLiteral &&
                it->getLexeme() != Lexeme::StringLiteral)
            {
                errors = std::format("line:{}: error: identifier '{}' has invalid value type", attribute->getNumLine(),
                                     attribute->getContent());
                successful = false;
            }

            attributes[std::string(attribute->getContent())] = std::string(it->getContent());

            it++;

            if (it->getLexeme() != Lexeme::RightParen)
            {
                errors = std::format("line:{}: error: identifier '{}' missing missing closing (')') character",
                                     attribute->getNumLine(), attribute->getContent());
                successful = false;
            }

            it++;

            if (it->getLexeme() == Lexeme::Comma)
            {
                it++;
            }
        }

        it++;

        if (it->getContent().compare("PS") == 0)
        {
            output.psAttributes = std::move(attributes);
        }
        else if (it->getContent().compare("CS") == 0)
        {
            output.csAttributes = std::move(attributes);
        }

        return it;
    }

    auto Parser::parseDataGroup(std::span<Token const>::iterator it, bool& successful, ShaderParseData& output)
        -> std::span<Token const>::iterator
    {
        auto region = it;
        it++;

        if (it->getLexeme() != Lexeme::LeftBrace)
        {
            errors = std::format("line:{}: error: identifier '{}' missing end of scope", region->getNumLine(),
                                 region->getContent());
            successful = false;
        }

        it++;

        output.effectStructure.name = "EffectData";

        while (it != tokens.end() && it->getLexeme() != Lexeme::RightBrace)
        {
            asset::fx::StructureElementData outVariable{};
            it = parseStructVariable(it, successful, outVariable);
            output.effectStructure.size += asset::fx::sizeof_ElementType(outVariable.type);
            output.effectStructure.elements.emplace_back(outVariable);
        }

        return ++it;
    }

    auto Parser::parseStructVariable(std::span<Token const>::iterator it, bool& successful,
                                     asset::fx::StructureElementData& output) -> std::span<Token const>::iterator
    {
        if (it->getLexeme() != Lexeme::FixedType)
        {
            errors = std::format("line:{}: error: invalid variable type '{}'", it->getNumLine(), it->getContent());
            successful = false;
        }

        auto typeResult = core::deserialize<core::serialize_ienum, asset::fx::ElementType>(
            std::istringstream(std::string(it->getContent())));
        assert(typeResult.has_value() && "missing field in serializable_enum<asset::fx::ElementType>");

        output.type = typeResult.value();

        it++;

        if (it->getLexeme() != Lexeme::Identifier)
        {
            errors =
                std::format("line:{}: error: identifier '{}' missing end of scope", it->getNumLine(), it->getContent());
            successful = false;
        }

        auto variable = it;

        output.name = std::string(it->getContent());

        it++;

        if (it->getLexeme() != Lexeme::Semicolon)
        {
            errors = std::format("line:{}: error: identifier '{}' missing ending (;) character", variable->getNumLine(),
                                 variable->getContent());
            successful = false;
        }

        return ++it;
    }

    auto Parser::parseHeaderGroup(std::span<Token const>::iterator it, bool& successful, ShaderParseData& output)
        -> std::span<Token const>::iterator
    {
        auto region = it;
        it++;

        if (it->getLexeme() != Lexeme::LeftBrace)
        {
            errors = std::format("line:{}: error: identifier '{}' missing end of scope", region->getNumLine(),
                                 region->getContent());
            successful = false;
        }

        it++;

        while (it != tokens.end() && it->getLexeme() != Lexeme::RightBrace)
        {
            std::string outVariable;
            it = this->parseOptionKey(it, successful, outVariable);

            if (outVariable.compare("Name") == 0)
            {
                std::string outValue;
                it = this->parseOptionValue(it, successful, outValue);
                output.header.name = outValue;
            }
            else if (outVariable.compare("Description") == 0)
            {
                std::string outValue;
                it = this->parseOptionValue(it, successful, outValue);
                output.header.description = outValue;
            }
            else if (outVariable.compare("Domain") == 0)
            {
                std::string outValue;
                it = this->parseOptionValue(it, successful, outValue);
                output.header.domain = outValue;
            }
            else if (outVariable.compare("Blend") == 0)
            {
                std::string outValue;
                it = this->parseOptionValue(it, successful, outValue);
                output.header.blend = outValue;
            }
            else if (outVariable.compare("Features") == 0)
            {
                std::vector<std::string> outValues;
                it = this->parseOptionValue(it, successful, outValues);
                output.header.features = std::move(outValues);
            }
            else
            {
                errors = std::format("line:{}: error: unknown option '{}'", region->getNumLine(), outVariable);
                successful = false;
            }
        }

        return ++it;
    }

    auto Parser::parseShaderCode(std::span<Token const>::iterator it, bool& successful, ShaderParseData& output)
        -> std::span<Token const>::iterator
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
        else
        {
            errors =
                std::format("line:{}: error: unknown shader stage '{}'", region->getNumLine(), region->getContent());
            successful = false;
        }

        it++;

        if (it->getLexeme() != Lexeme::LeftBrace)
        {
            errors = std::format("line:{}: error: identifier '{}' missing end of scope", region->getNumLine(),
                                 region->getContent());
            successful = false;
        }

        it++;

        if (it->getLexeme() != Lexeme::ShaderCode)
        {
            errors = std::format("line:{}: error: identifier '{}' missing end of scope", region->getNumLine(),
                                 region->getContent());
            successful = false;
        }

        output.shaderCodes[stageType] = std::string(it->getContent());

        it++;

        if (it->getLexeme() != Lexeme::RightBrace)
        {
            errors = std::format("line:{}: error: identifier '{}' missing end of scope", region->getNumLine(),
                                 region->getContent());
            successful = false;
        }

        return ++it;
    }

    auto Parser::parseOptionKey(std::span<Token const>::iterator it, bool& successful, std::string& output)
        -> std::span<Token const>::iterator
    {
        if (it->getLexeme() != Lexeme::Identifier)
        {
            errors =
                std::format("line:{}: error: identifier '{}' missing end of scope", it->getNumLine(), it->getContent());
            successful = false;
        }

        auto variable = it;
        output = variable->getContent();

        it++;

        if (it->getLexeme() != Lexeme::Assignment)
        {
            errors = std::format("line:{}: error: identifier '{}' missing assignment (=) operator",
                                 variable->getNumLine(), variable->getContent());
            successful = false;
        }

        return ++it;
    }

    auto Parser::parseToken(std::span<Token const>::iterator it, ShaderParseData& output) -> bool
    {
        bool successful = true;

        if (it->getLexeme() == Lexeme::Identifier)
        {
            if (it->getContent().compare("HEADER") == 0)
            {
                it = this->parseHeaderGroup(it, successful, output);
            }
            else if (it->getContent().compare("DATA") == 0)
            {
                it = this->parseDataGroup(it, successful, output);
            }
            else if (it->getContent().compare("VS") == 0 || it->getContent().compare("PS") == 0 ||
                     it->getContent().compare("CS") == 0)
            {
                it = this->parseShaderCode(it, successful, output);
            }
            else
            {
                errors = std::format("line:{}: error: unknown section '{}'", it->getNumLine(), it->getContent());
                return false;
            }
        }
        else if (it->getLexeme() == Lexeme::LeftBracket)
        {
            it = this->parseAttributes(it, successful, output);
        }
        else
        {
            errors = std::format("line:{}: error: expression is incomplete", it->getNumLine());
            return false;
        }

        if (!successful)
        {
            return false;
        }

        if (it != tokens.end())
        {
            return this->parseToken(it, output);
        }
        else
        {
            return true;
        }
    }

    auto Parser::parse(std::span<Token const> const tokens) -> std::expected<ShaderParseData, core::error>
    {
        this->tokens = tokens;

        ShaderParseData shaderParseData{};
        if (!this->parseToken(tokens.begin(), shaderParseData))
        {
            return std::unexpected(core::error(errors));
        }
        else
        {
            return shaderParseData;
        }
    }
} // namespace ionengine::shadersys