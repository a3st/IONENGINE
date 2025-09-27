// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/error.hpp"
#include "core/string_utils.hpp"
#include "fx.hpp"
#include "lexer.hpp"

namespace ionengine::shadersys
{
    namespace internal
    {
        template <typename>
        struct is_std_vector : std::false_type
        {
        };

        template <typename T, typename A>
        struct is_std_vector<std::vector<T, A>> : std::true_type
        {
        };
    } // namespace internal

    struct ShaderParseData
    {
        asset::fx::HeaderData headerData;
        asset::fx::StructureData effectData;
        std::unordered_map<asset::fx::StageType, std::string> shaderData;
        std::unordered_map<std::string, std::string> csAttributes;
        std::unordered_map<std::string, std::string> psAttributes;
    };

    class Parser
    {
      public:
        Parser() = default;

        auto parse(std::span<Token const> const tokens) -> std::expected<ShaderParseData, core::error>;

      private:
        std::string errors;
        std::span<Token const> tokens;

        auto parseToken(std::span<Token const>::iterator it, ShaderParseData& parseData) -> bool;

        auto parseHeaderGroup(std::span<Token const>::iterator it, bool& successful, ShaderParseData& parseData)
            -> std::span<Token const>::iterator;

        auto parseDataGroup(std::span<Token const>::iterator it, bool& successful, ShaderParseData& parseData)
            -> std::span<Token const>::iterator;

        auto parseAttributes(std::span<Token const>::iterator it, bool& successful, ShaderParseData& parseData)
            -> std::span<Token const>::iterator;

        auto parseShaderCode(std::span<Token const>::iterator it, bool& successful, ShaderParseData& parseData)
            -> std::span<Token const>::iterator;

        auto parseOptionKey(std::span<Token const>::iterator it, bool& successful, std::string& outKey)
            -> std::span<Token const>::iterator;

        template <typename Type>
        auto parseOptionValue(std::span<Token const>::iterator it, bool& successful, Type& outValue)
            -> std::span<Token const>::iterator
        {
            if (it->getLexeme() == Lexeme::FloatLiteral || it->getLexeme() == Lexeme::BoolLiteral ||
                it->getLexeme() == Lexeme::StringLiteral)
            {
                if constexpr (std::is_integral_v<Type> || std::is_same_v<Type, bool> || std::is_floating_point_v<Type>)
                {
                    auto result = core::string_utils::lexical_cast<Type>(it->getContent());
                    if (!result.has_value())
                    {
                        errors = std::format("line:{}: error: {}", it->getNumLine(), result.error().what());
                        successful = false;
                    }

                    outValue = result.value();
                }
                else if constexpr (std::is_same_v<
                                       Type, std::basic_string<char, std::char_traits<char>, std::allocator<char>>>)
                {
                    outValue = it->getContent();
                }
            }
            else if (it->getLexeme() == Lexeme::LeftBracket)
            {
                it++;

                while (it != tokens.end() && it->getLexeme() != Lexeme::RightBracket)
                {
                    if (it->getLexeme() == Lexeme::StringLiteral)
                    {
                        if constexpr (internal::is_std_vector<Type>::value)
                        {
                            outValue.emplace_back(it->getContent());
                        }
                    }

                    it++;

                    if (it->getLexeme() == Lexeme::Comma)
                    {
                        it++;
                    }
                }
            }
            else
            {
                errors = std::format("line:{}: error: identifier '{}' has invalid value type", it->getNumLine(),
                                     it->getContent());
                successful = false;
            }

            it++;

            if (it->getLexeme() != Lexeme::Semicolon)
            {
                errors = std::format("line:{}: error: identifier '{}' missing ending (;) character", it->getNumLine(),
                                     it->getContent());
                successful = false;
            }

            return ++it;
        }

        auto parseStructVariable(std::span<Token const>::iterator it, bool& successful,
                                 asset::fx::StructureElementData& outVariable) -> std::span<Token const>::iterator;
    };
} // namespace ionengine::shadersys