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
        LeftBracket,
        RightBracket,
        Comma,
        Colon,
        StringLiteral,
        FloatLiteral,
        FixedType,
        Namespace,
        Arrow,
        Plus,
        PlusEqual,
        Minus,
        MinusEqual,
        Multiply,
        MultiplyEqual,
        Slash,
        SlashEqual,
        Ampersand,
        AmpersandEqual,
        LeftShift,
        RightShift,
        Less,
        LessEqual,
        Greater,
        GreaterEqual,
        Not,
        NotEqual,
        Equal,
        And,
        Or,
        Pipe,
        PipeEqual
    };

    class Token
    {
      public:
        Token(std::string_view const str, Lexeme const lexeme);

        Token(Token const& other);

        Token(Token&& other);

        auto operator=(Token const& other) -> Token&;

        auto operator=(Token&& other) -> Token&;

        auto getLexeme() const -> Lexeme;

        auto getContent() const -> std::string_view;

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