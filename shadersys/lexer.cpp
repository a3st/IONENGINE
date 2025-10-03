// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "lexer.hpp"
#include "core/string_utils.hpp"
#include "precompiled.h"

namespace ionengine::shadersys
{
    std::set<std::string> const lexerTypes{"uint",   "bool",     "float",    "float2",  "float3",
                                           "float4", "float2x2", "float3x3", "float4x4"};

    Token::Token(std::string_view const str, Lexeme const lexeme, std::filesystem::path const& filePath,
                 uint32_t const numLine)
        : str(str), lexeme(lexeme), filePath(filePath), numLine(numLine)
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

    auto Token::getContent() const -> std::string const&
    {
        return str;
    }

    auto Token::getNumLine() const -> uint32_t
    {
        return numLine;
    }

    auto Token::getFilePath() const -> std::filesystem::path const&
    {
        return filePath;
    }

    Lexer::Lexer() : locale("en_US.utf8")
    {
    }

    auto Lexer::parse(std::filesystem::path const& filePath) -> std::expected<std::vector<Token>, core::error>
    {
        std::ifstream stream(filePath);
        if (!stream.is_open())
        {
            return std::unexpected(core::error("Failed to open file"));
        }

        std::string const buffer = {std::istreambuf_iterator<char>(stream.rdbuf()), {}};
        std::vector<Token> tokens;
        this->analyzeBufferData(buffer, filePath, tokens);
        return tokens;
    }

    auto Lexer::analyzeBufferData(std::string_view const buffer, std::filesystem::path const& filePath,
                                  std::vector<Token>& tokens) -> void
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
                Token token(tokenStr, tokenLexeme, filePath, curLine);
                tokens.emplace_back(std::move(token));
            }

            offset++;
        }
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
        return lexerTypes.find(std::string(str)) != lexerTypes.end();
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