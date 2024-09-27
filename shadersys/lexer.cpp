// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "lexer.hpp"
#include "core/string.hpp"
#include "precompiled.h"

namespace ionengine::shadersys
{
    std::set<std::string> const types{"uint",   "bool",     "float",    "float2",  "float3",
                                      "float4", "float2x2", "float3x3", "float4x4"};

    Token::Token(std::string_view const str, Lexeme const lexeme, uint32_t const numLine)
        : str(str), lexeme(lexeme), numLine(numLine)
    {
    }

    Token::Token(Token const& other) : str(other.str), lexeme(other.lexeme), numLine(other.numLine)
    {
    }

    Token::Token(Token&& other) : str(other.str), lexeme(other.lexeme), numLine(other.numLine)
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

    auto Token::getLexeme() const -> Lexeme
    {
        return lexeme;
    }

    auto Token::getContent() const -> std::string_view
    {
        return str;
    }

    auto Token::getNumLine() const -> uint32_t
    {
        return numLine;
    }

    Lexer::Lexer(std::string_view const input) : locale("en_US.utf8")
    {
        this->analyzeBufferData(input);
    }

    auto Lexer::analyzeBufferData(std::string_view const buffer) -> void
    {
        uint64_t offset = 0;
        uint32_t curLine = 1;
        std::stack<Lexeme> seqLexeme;

        while (offset < buffer.size())
        {
            Lexeme tokenLexeme = Lexeme::Unknown;
            std::string_view tokenStr;

            switch (buffer[offset])
            {
                case '\n': {
                    curLine++;
                    break;
                }
                case '{': {
                    if (!seqLexeme.empty() && seqLexeme.top() == Lexeme::ShaderCode)
                    {
                        tokenLexeme = Lexeme::LeftBrace;
                        seqLexeme.emplace(tokenLexeme);
                    }
                    else
                    {
                        tokenLexeme = Lexeme::LeftBrace;
                    }
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
                case '[': {
                    tokenLexeme = Lexeme::LeftBracket;
                    break;
                }
                case ']': {
                    tokenLexeme = Lexeme::RightBracket;
                    break;
                }
                case ',': {
                    tokenLexeme = Lexeme::Comma;
                    break;
                }
                case '=': {
                    tokenLexeme = Lexeme::Assignment;
                    break;
                }
                case ':': {
                    tokenLexeme = Lexeme::Colon;
                    break;
                }
                case '-': {
                    tokenLexeme = Lexeme::Minus;
                    break;
                }
                case '/': {
                    tokenLexeme = Lexeme::Slash;

                    if (buffer[offset + 1] == '/')
                    {
                        offset += 2;
                        
                        while (offset < buffer.size() && buffer[offset] != '\n')
                        {
                            offset++;
                        }

                        tokenLexeme = Lexeme::Unknown;
                    }
                    break;
                }
                case '*': {
                    tokenLexeme = Lexeme::Multiply;
                    break;
                }
                case ';': {
                    tokenLexeme = Lexeme::Semicolon;
                    break;
                }
                case '+': {
                    tokenLexeme = Lexeme::Plus;
                    break;
                }
                case '\"': {
                    offset++;

                    uint64_t const tokenStart = offset;

                    while (offset < buffer.size() && buffer[offset] != '\"')
                    {
                        offset++;
                    }

                    tokenLexeme = Lexeme::StringLiteral;
                    tokenStr = std::string_view(buffer.data() + tokenStart, buffer.data() + offset);
                    break;
                }
                default: {
                    if (!seqLexeme.empty() && seqLexeme.top() == Lexeme::LeftBrace)
                    {
                        offset++;

                        uint64_t const tokenStart = offset;
                        bool isEnd = false;

                        while (offset < buffer.size())
                        {
                            if (isEnd && buffer[offset] == '}')
                            {
                                break;
                            }
                            else if (!isEnd && buffer[offset] == '}')
                            {
                                isEnd = true;
                            }
                            else if (isEnd && buffer[offset] == '{')
                            {
                                isEnd = false;
                            }

                            offset++;
                        }

                        if (isEnd)
                        {
                            seqLexeme.pop();
                            tokenLexeme = seqLexeme.top();
                            tokenStr = std::string_view(buffer.data() + tokenStart, buffer.data() + offset);
                            seqLexeme.pop();
                        }

                        offset--;
                    }
                    else if (this->isNumeric(buffer[offset]))
                    {
                        uint64_t const tokenStart = offset;

                        while (this->isNumeric(buffer[offset]))
                        {
                            offset++;

                            if (buffer[offset] == '.')
                            {
                                offset++;

                                while (this->isNumeric(buffer[offset]))
                                {
                                    offset++;
                                }
                            }
                        }

                        tokenLexeme = Lexeme::FloatLiteral;
                        tokenStr = std::string_view(buffer.data() + tokenStart, buffer.data() + offset);

                        offset--;
                    }
                    else if (this->isLetter(buffer[offset]))
                    {
                        uint64_t const tokenStart = offset;

                        while (offset < buffer.size() && (this->isLetter(buffer[offset]) ||
                                                          this->isNumeric(buffer[offset]) || buffer[offset] == '_'))
                        {
                            offset++;
                        }

                        tokenStr = std::string_view(buffer.data() + tokenStart, buffer.data() + offset);

                        if (tokenStr.compare("false") == 0 || tokenStr.compare("true") == 0)
                        {
                            tokenLexeme = Lexeme::BoolLiteral;
                        }
                        else if (this->isType(tokenStr))
                        {
                            tokenLexeme = Lexeme::FixedType;
                        }
                        else
                        {
                            if (this->isShaderCode(tokenStr))
                            {
                                seqLexeme.emplace(Lexeme::ShaderCode);
                            }

                            tokenLexeme = Lexeme::Identifier;
                        }

                        offset--;
                    }
                    break;
                }
            }

            if (tokenLexeme != Lexeme::Unknown)
            {
                Token token(tokenStr, tokenLexeme, curLine);
                tokens.emplace_back(std::move(token));
                // std::cout << std::format("Token {}: {}", (uint8_t)tokenLexeme, tokenStr) << std::endl;
            }

            offset++;
        }
    }

    auto Lexer::getTokens() const -> std::span<Token const>
    {
        return tokens;
    }

    auto Lexer::isLetter(char const c) const -> bool
    {
        return std::isalpha(c, locale);
    }

    auto Lexer::isNumeric(char const c) const -> bool
    {
        return std::isdigit(c, locale);
    }

    auto Lexer::isType(std::string_view const str) const -> bool
    {
        return types.find(std::string(str)) != types.end();
    }

    auto Lexer::isShaderCode(std::string_view const str) const -> bool
    {
        if (str.compare("VS") == 0 || str.compare("PS") == 0 || str.compare("CS") == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
} // namespace ionengine::shadersys