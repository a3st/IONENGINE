// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/error.hpp"
#include "core/string.hpp"
#include "fx.hpp"
#include "lexer.hpp"
#undef EOF

namespace ionengine::shadersys
{
    /*!
        \brief Errors that will get when parse a shader file
    */
    enum class ParseError : uint32_t
    {
        EOF = 1000,
        Operator = 1001,
        Character = 1002,
        UnknownType = 1003,
        OtherType = 1004,
        ShaderCode = 1005,
        UnknownSection = 1006,
        EOS = 1007
    };

    /*!
        \brief Data that got when parsed a shader file
    */
    struct ShaderParseData
    {
        asset::fx::HeaderData headerData;
        std::unordered_map<asset::fx::StageType, std::string> codeData;
        asset::fx::StructureData materialData;
        std::unordered_map<std::string, std::string> csAttributes;
        std::unordered_map<std::string, std::string> psAttributes;
    };

    /*!
        \brief Parser that can parse a FX shader file
    */
    class Parser
    {
      public:
        Parser(std::string& errors);

        auto parse(Lexer const& lexer) -> std::expected<ShaderParseData, ParseError>;

      private:
        std::string* errors;
        std::optional<ParseError> parseError;

        auto parseToken(std::span<Token const>::iterator it) -> std::expected<ShaderParseData, ParseError>;

        auto parseHeader(std::span<Token const>::iterator it) -> std::span<Token const>::iterator;

        auto parseData(std::span<Token const>::iterator it) -> std::span<Token const>::iterator;

        auto parseAttributes(std::span<Token const>::iterator it) -> std::span<Token const>::iterator;

        auto parseShaderCode(std::span<Token const>::iterator it) -> std::span<Token const>::iterator;

        auto parseOptionValue(std::span<Token const>::iterator it) -> std::span<Token const>::iterator;

        /*template <typename Type>
        auto parseOptionValue(std::span<Token const>::iterator variable, std::span<Token const>::iterator it,
                              Type& value) -> std::span<Token const>::iterator
        {
            if constexpr (std::is_integral_v<Type> && !std::is_same_v<Type, bool>)
            {
                if (it->getLexeme() == Lexeme::FloatLiteral)
                {
                    try
                    {
                        value = core::ston<Type>(it->getContent());
                    }
                    catch (core::runtime_error e)
                    {
                        throw parser_error(std::format(
                            "line:{}: error {}: identifier '{}' has unknown value type", variable->getNumLine(),
                            std::to_underlying(ErrorCode::UnknownType), variable->getContent()));
                    }
                }
                else
                {
                    throw parser_error(std::format("line:{}: error {}: identifier '{}' has other value type",
                                                   variable->getNumLine(), std::to_underlying(ErrorCode::OtherType),
                                                   variable->getContent()));
                }
            }
            else if constexpr (std::is_same_v<Type,
                                              std::basic_string<char, std::char_traits<char>, std::allocator<char>>>)
            {
                if (it->getLexeme() == Lexeme::StringLiteral)
                {
                    value = it->getContent();
                }
                else
                {
                    throw parser_error(std::format("line:{}: error {}: identifier '{}' has other value type",
                                                   variable->getNumLine(), std::to_underlying(ErrorCode::OtherType),
                                                   variable->getContent()));
                }
            }
            else if constexpr (std::is_integral_v<Type> && std::is_same_v<Type, bool>)
            {
                if (it->getLexeme() == Lexeme::BoolLiteral)
                {
                    value =
                        it->getContent().compare("true") ? true
                        : it->getContent().compare("false")
                            ? false
                            : throw parser_error(std::format(
                                  "line:{}: error {}: identifier '{}' has unknown value type", variable->getNumLine(),
                                  std::to_underlying(ErrorCode::UnknownType), variable->getContent()));
                }
                else
                {
                    throw parser_error(std::format("line:{}: error {}: identifier '{}' has other value type",
                                                   variable->getNumLine(), std::to_underlying(ErrorCode::OtherType),
                                                   variable->getContent()));
                }
            }

            it++;

            if (it->getLexeme() != Lexeme::Semicolon)
            {
                throw parser_error(std::format("line:{}: error {}: missing ';' character", variable->getNumLine(),
                                               std::to_underlying(ErrorCode::Character)));
            }

            it++;
            return it;
        }*/
    };
} // namespace ionengine::shadersys