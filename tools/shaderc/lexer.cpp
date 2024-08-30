// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "lexer.hpp"
#include "core/string.hpp"
#include "precompiled.h"

namespace ionengine::tools::shaderc
{
    std::set<std::string> const types{"uint",     "bool",     "float",    "float2",    "float3",    "float4",
                                      "float2x2", "float3x3", "float4x4", "Texture2D", "Texture3D", "SamplerState"};

    std::set<std::string> const keywords{"import", "struct", "technique", "return", "hlsl"};

    std::set<std::string> const attributes{"register", "space", "constant", "semantic"};

    Token::Token(std::string_view const str, Lexeme const lexeme, uint32_t const numLine)
        : str(str), lexeme(lexeme), numLine(numLine)
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
        uint32_t curLine = 0;

        while (stream)
        {
            curLine++;

            if (!getEndOfLineOffset(stream, offset))
            {
                break;
            }

            std::string_view const line(buffer.data() + cur, buffer.data() + offset);
            offset = 0;

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
                        if (line[offset + 1] == '=')
                        {
                            tokenLexeme = Lexeme::Equal;
                            offset++;
                        }
                        else
                        {
                            tokenLexeme = Lexeme::Assignment;
                        }
                        break;
                    }
                    case ':': {
                        if (line[offset + 1] == ':')
                        {
                            tokenLexeme = Lexeme::Namespace;
                            offset++;
                        }
                        else
                        {
                            tokenLexeme = Lexeme::Colon;
                        }
                        break;
                    }
                    case '|': {
                        if (line[offset + 1] == '|')
                        {
                            tokenLexeme = Lexeme::Or;
                            offset++;
                        }
                        else if (line[offset + 1] == '=')
                        {
                            tokenLexeme = Lexeme::PipeEqual;
                            offset++;
                        }
                        else
                        {
                            tokenLexeme = Lexeme::Pipe;
                        }
                        break;
                    }
                    case '<': {
                        if (line[offset + 1] == '<')
                        {
                            tokenLexeme = Lexeme::LeftShift;
                            offset++;
                        }
                        else if (line[offset + 1] == '=')
                        {
                            tokenLexeme = Lexeme::LessEqual;
                            offset++;
                        }
                        else
                        {
                            tokenLexeme = Lexeme::Less;
                        }
                        break;
                    }
                    case '>': {
                        if (line[offset + 1] == '>')
                        {
                            tokenLexeme = Lexeme::RightShift;
                            offset++;
                        }
                        else if (line[offset + 1] == '=')
                        {
                            tokenLexeme = Lexeme::GreaterEqual;
                            offset++;
                        }
                        else
                        {
                            tokenLexeme = Lexeme::Greater;
                        }
                        break;
                    }
                    case '!': {
                        if (line[offset + 1] == '=')
                        {
                            tokenLexeme = Lexeme::NotEqual;
                            offset++;
                        }
                        else
                        {
                            tokenLexeme = Lexeme::Not;
                        }
                        break;
                    }
                    case '-': {
                        if (line[offset + 1] == '>')
                        {
                            tokenLexeme = Lexeme::Arrow;
                            offset++;
                        }
                        else if (line[offset + 1] == '=')
                        {
                            tokenLexeme = Lexeme::MinusEqual;
                            offset++;
                        }
                        else
                        {
                            tokenLexeme = Lexeme::Minus;
                        }
                        break;
                    }
                    case '/': {
                        if (line[offset + 1] == '=')
                        {
                            tokenLexeme = Lexeme::SlashEqual;
                            offset++;
                        }
                        else
                        {
                            tokenLexeme = Lexeme::Slash;
                        }
                        break;
                    }
                    case '*': {
                        if (line[offset + 1] == '=')
                        {
                            tokenLexeme = Lexeme::MultiplyEqual;
                            offset++;
                        }
                        else
                        {
                            tokenLexeme = Lexeme::Multiply;
                        }
                        break;
                    }
                    case ';': {
                        tokenLexeme = Lexeme::Semicolon;
                        break;
                    }
                    case '&': {
                        if (line[offset + 1] == '=')
                        {
                            tokenLexeme = Lexeme::AmpersandEqual;
                            offset++;
                        }
                        else if (line[offset + 1] == '&')
                        {
                            tokenLexeme = Lexeme::And;
                            offset++;
                        }
                        else
                        {
                            tokenLexeme = Lexeme::Ampersand;
                        }
                        break;
                    }
                    case '+': {
                        if (line[offset + 1] == '=')
                        {
                            tokenLexeme = Lexeme::PlusEqual;
                            offset++;
                        }
                        else
                        {
                            tokenLexeme = Lexeme::Plus;
                        }
                        break;
                    }
                    case '\"': {
                        offset++;

                        uint64_t tokenStart = offset;

                        while (line[offset] != '\"')
                        {
                            offset++;
                        }

                        tokenLexeme = Lexeme::StringLiteral;
                        tokenStr = std::string_view(line.data() + tokenStart, line.data() + offset);
                        break;
                    }
                    default: {
                        if (this->isNumeric(line[offset]))
                        {
                            uint64_t tokenStart = offset;

                            while (this->isNumeric(line[offset]))
                            {
                                offset++;

                                if (line[offset] == '.')
                                {
                                    offset++;

                                    while (this->isNumeric(line[offset]))
                                    {
                                        offset++;
                                    }
                                }
                            }

                            tokenLexeme = Lexeme::FloatLiteral;
                            tokenStr = std::string_view(line.data() + tokenStart, line.data() + offset);

                            offset--;
                        }
                        else if (this->isLetter(line[offset]))
                        {
                            uint64_t tokenStart = offset;

                            while (this->isLetter(line[offset]) || this->isNumeric(line[offset]) || line[offset] == '_')
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
                            else if (this->isAttribute(tokenStr))
                            {
                                tokenLexeme = Lexeme::Attribute;
                            }
                            else
                            {
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

            cur = stream.tellg();
        }
    }

    auto Lexer::getFilePath() const -> std::filesystem::path const&
    {
        return filePath;
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

    auto Lexer::isType(std::string_view const str) const -> bool
    {
        return types.find(std::string(str)) != types.end();
    }

    auto Lexer::isKeyword(std::string_view const str) const -> bool
    {
        return keywords.find(std::string(str)) != keywords.end();
    }

    auto Lexer::isAttribute(std::string_view const str) const -> bool
    {
        return attributes.find(std::string(str)) != attributes.end();
    }
} // namespace ionengine::tools::shaderc