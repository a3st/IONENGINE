// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/error.hpp"
#include "core/string.hpp"
#include "fx.hpp"
#include "lexer.hpp"

namespace ionengine::shadersys
{
    enum class ErrorCode : uint32_t
    {
        EndOfScope = 1000,
        Operator = 1001,
        Character = 1002,
        UnknownType = 1003,
        OtherType = 1004,
        ShaderCode = 1005
    };

    class parser_error : public core::runtime_error
    {
      public:
        parser_error(std::string_view const error) : core::runtime_error(error)
        {
        }
    };

    class Parser
    {
      public:
        Parser() = default;

        auto parse(Lexer const& lexer, fx::ShaderHeaderData& headerData, fx::ShaderOutputData& outputData,
                   std::unordered_map<fx::ShaderStageType, std::string>& stageData) -> bool;

      private:
        template <typename Type>
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
                            "line {}: error {}: Identifier '{}' has unknown value type", variable->getNumLine(),
                            std::to_underlying(ErrorCode::UnknownType), variable->getContent()));
                    }
                }
                else
                {
                    throw parser_error(std::format("line {}: error {}: Identifier '{}' has other value type",
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
                    throw parser_error(std::format("line {}: error {}: Identifier '{}' has other value type",
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
                                  "line {}: error {}: Identifier '{}' has unknown value type", variable->getNumLine(),
                                  std::to_underlying(ErrorCode::UnknownType), variable->getContent()));
                }
                else
                {
                    throw parser_error(std::format("line {}: error {}: Identifier '{}' has other value type",
                                                   variable->getNumLine(), std::to_underlying(ErrorCode::OtherType),
                                                   variable->getContent()));
                }
            }

            it++;

            if (it->getLexeme() != Lexeme::Semicolon)
            {
                throw parser_error(std::format("line {}: error {}: missing ';' character", variable->getNumLine(),
                                               std::to_underlying(ErrorCode::Character)));
            }

            it++;
            return it;
        }
    };
} // namespace ionengine::shadersys