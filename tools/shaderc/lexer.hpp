// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::tools::shaderc
{
    enum class Lexeme : uint8_t
    {
        Unknown,
        Semicolon,
        Keyword,
        Identifier,
        Assignment,
        LeftBrace,
        RightBrace,
        LeftParen,
        RightParen,
        Comma,
        Colon,
        StringLiteral,
        FloatLiteral,
        FixedType,
        Namespace,
        Arrow
    };

    class Token
    {
      public:
        Token(std::string_view const str, Lexeme const lexeme);

        Token(Token const& other);

        Token(Token&& other);

        auto operator=(Token const& other) -> Token&;

        auto operator=(Token&& other) -> Token&;

      private:
        std::string_view str;
        Lexeme lexeme;
    };

    class Lexer
    {
      public:
        Lexer(std::string_view const dataBytes);

        auto getTokens() const -> std::span<Token const>;

      private:
        std::string buffer;
        std::vector<Token> tokens;

        std::locale locale;

        auto isLetter(char c) const -> bool;

        auto isNumeric(char c) const -> bool;

        auto isType(std::string_view const str) -> bool;

        auto isKeyword(std::string_view const str) -> bool;
    };
} // namespace ionengine::tools::shaderc