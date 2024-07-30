// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "lexer.hpp"
#include "core/string.hpp"
#include "precompiled.h"

namespace ionengine::tools::shaderc
{
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

    Lexer::Lexer(std::string_view const dataBytes) : buffer(dataBytes)
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

            uint64_t tokenOffset = 0;
            bool isTokenStart = false;

            for (uint64_t const i : std::views::iota(0u, line.size()))
            {
                if (line[i] == ' ')
                {
                    continue;
                }

                if (!isTokenStart)
                {
                    switch (line[i])
                    {
                        case '=': {
                            std::string_view const tokenExpr(line.data() + i, line.data() + i + 1);

                            Token token(tokenExpr, Lexeme::Assignment);
                            tokens.emplace_back(std::move(token));
                            break;
                        }
                        case '\"': {
                            tokenOffset = i;
                            isTokenStart = true;
                            break;
                        }
                        case ',': {
                            std::string_view const tokenExpr(line.data() + i, line.data() + i + 1);

                            Token token(tokenExpr, Lexeme::Comma);
                            tokens.emplace_back(std::move(token));
                            break;
                        }
                        case '{': {
                            std::string_view const tokenExpr(line.data() + i, line.data() + i + 1);

                            Token token(tokenExpr, Lexeme::LeftBrace);
                            tokens.emplace_back(std::move(token));
                            break;
                        }
                        case '(': {
                            std::string_view const tokenExpr(line.data() + i, line.data() + i + 1);

                            Token token(tokenExpr, Lexeme::LeftParen);
                            tokens.emplace_back(std::move(token));
                            break;
                        }
                        default: {
                            std::string_view const tokenExpr(line.data() + tokenOffset, line.data() + i + 1);

                            uint64_t foundOffset = 0;
                            if ((foundOffset = tokenExpr.find("import")) != std::string::npos)
                            {
                                std::string_view const tokenExpr(line.data() + foundOffset, line.data() + i + 1);

                                Token token(tokenExpr, Lexeme::StringLiteral);
                                tokens.emplace_back(std::move(token));
                                tokenOffset = i;

                                std::cout << tokenExpr << std::endl;
                            }
                        }
                    }
                }
                else
                {
                    switch (line[i])
                    {
                        case '\"': {
                            std::string_view const tokenExpr(line.data() + tokenOffset + 1, line.data() + i);

                            Token token(tokenExpr, Lexeme::StringLiteral);
                            tokens.emplace_back(std::move(token));
                            isTokenStart = false;

                            std::cout << tokenExpr << std::endl;
                            break;
                        }
                        case '}': {
                            std::string_view const tokenExpr(line.data() + i, line.data() + i + 1);

                            Token token(tokenExpr, Lexeme::RightBrace);
                            tokens.emplace_back(std::move(token));
                            break;
                        }
                        case ')': {
                            std::string_view const tokenExpr(line.data() + i, line.data() + i + 1);

                            Token token(tokenExpr, Lexeme::RightParen);
                            tokens.emplace_back(std::move(token));
                            break;
                        }
                    }
                }
            }

            cur = stream.tellg();
        }
    }

    auto Lexer::getTokens() const -> std::span<Token const>
    {
        return tokens;
    }
} // namespace ionengine::tools::shaderc