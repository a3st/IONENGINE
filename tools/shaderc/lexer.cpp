// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "lexer.hpp"
#include "core/string.hpp"
#include "precompiled.h"

namespace ionengine::tools::shaderc
{
    std::set<std::string> const types{"uint",     "bool",     "float",    "float2",    "float3",   "float4",
                                      "float2x2", "float3x3", "float4x4", "Texture2D", "Texture3D"};

    std::set<std::string> const keywords{"import", "struct", "technique", "return"};

    Token::Token(std::string_view const str, Lexeme const lexeme) : str(str), lexeme(lexeme)
    {
    }

    Token::Token(Token const& other) : str(other.str), lexeme(other.lexeme)
    {
    }

    Token::Token(Token&& other) : str(other.str), lexeme(other.lexeme)
    {
    }

    auto Token::operator=(Token const& other) -> Token&
    {
        str = other.str;
        lexeme = other.lexeme;
        return *this;
    }

    auto Token::operator=(Token&& other) -> Token&
    {
        str = other.str;
        lexeme = other.lexeme;
        return *this;
    }

    Lexer::Lexer(std::string_view const dataBytes) : buffer(dataBytes), locale("en_US.utf8")
    {
        std::basic_ispanstream<char> stream(std::span<char const>(buffer.data(), buffer.size()));

        auto getEndOfLineOffset = [&](std::istream& stream, uint64_t& out) -> bool {
            while (stream)
            {
                char ch = 0x1;
                if (!stream.read(&ch, sizeof(char)))
                {
                    return false;
                }

                if (ch == '\n')
                {
                    break;
                }
            }
            out = static_cast<uint64_t>(stream.tellg()) - 1;
            return true;
        };

        uint64_t cur = 0;
        uint64_t offset = 0;

        while (stream)
        {
            if (!getEndOfLineOffset(stream, offset))
            {
                break;
            }

            std::string_view const line(buffer.data() + cur, buffer.data() + offset);

            uint64_t offset = 0;

            while (offset != line.size())
            {
                Lexeme tokenLexeme = Lexeme::Unknown;
                std::string_view tokenStr;

                switch (line[offset])
                {
                    case '{': {
                        tokenLexeme = Lexeme::LeftBrace;
                        break;
                    }
                    case '}': {
                        tokenLexeme = Lexeme::RightBrace;
                        break;
                    }
                    case '(': {
                        tokenLexeme = Lexeme::LeftParen;
                        break;
                    }
                    case ')': {
                        tokenLexeme = Lexeme::RightParen;
                        break;
                    }
                    case '=': {
                        tokenLexeme = Lexeme::Assignment;
                        break;
                    }
                    case ':': {
                        tokenLexeme = Lexeme::Colon;
                        if (line[offset + 1] == ':')
                        {
                            tokenLexeme = Lexeme::Namespace;
                            offset++;
                        }
                        break;
                    }
                    case '-': {
                        if (line[offset + 1] == '>')
                        {
                            tokenLexeme = Lexeme::Arrow;
                            offset++;
                        }
                        break;
                    }
                    case ';': {
                        tokenLexeme = Lexeme::Semicolon;
                        break;
                    }
                    case '\"': {
                        offset++;

                        tokenLexeme = Lexeme::StringLiteral;
                        uint64_t tokenStart = offset;

                        while (line[offset] != '\"')
                        {
                            offset++;
                        }

                        tokenStr = std::string_view(line.data() + tokenStart, line.data() + offset);

                        offset++;
                        break;
                    }
                    default: {
                        if (this->isNumeric(line[offset]))
                        {
                            tokenLexeme = Lexeme::FloatLiteral;
                            uint64_t tokenStart = offset;

                            while (this->isNumeric(line[offset]))
                            {
                                offset++;
                            }

                            tokenStr = std::string_view(line.data() + tokenStart, line.data() + offset);

                            offset--;
                        }
                        else if (this->isLetter(line[offset]))
                        {
                            tokenLexeme = Lexeme::Identifier;
                            uint64_t tokenStart = offset;

                            while (this->isLetter(line[offset]) || this->isNumeric(line[offset]))
                            {
                                offset++;
                            }

                            tokenStr = std::string_view(line.data() + tokenStart, line.data() + offset);

                            if (this->isType(tokenStr))
                            {
                                tokenLexeme = Lexeme::FixedType;
                            }
                            else if (this->isKeyword(tokenStr))
                            {
                                tokenLexeme = Lexeme::Keyword;
                            }

                            offset--;
                        }
                        break;
                    }
                }

                if (tokenLexeme != Lexeme::Unknown)
                {
                    Token token(tokenStr, tokenLexeme);
                    tokens.emplace_back(std::move(token));

                    std::cout << std::format("Token {}: {}", (uint8_t)tokenLexeme, tokenStr) << std::endl;
                }

                offset++;
            }

            cur = stream.tellg();
        }
    }

    auto Lexer::getTokens() const -> std::span<Token const>
    {
        return tokens;
    }

    auto Lexer::isLetter(char c) const -> bool
    {
        return std::isalpha(c, locale);
    }

    auto Lexer::isNumeric(char c) const -> bool
    {
        return std::isdigit(c, locale);
    }

    auto Lexer::isType(std::string_view const str) -> bool
    {
        return types.find(std::string(str)) != types.end();
    }

    auto Lexer::isKeyword(std::string_view const str) -> bool
    {
        return keywords.find(std::string(str)) != keywords.end();
    }
} // namespace ionengine::tools::shaderc