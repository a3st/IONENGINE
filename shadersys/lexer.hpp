// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::shadersys
{
    enum class Lexeme : uint8_t
    {
        Unknown,
        Semicolon,
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
        BoolLiteral,
        FixedType,
        Plus,
        Minus,
        Multiply,
        Slash,
        ShaderCode
    };

    class Token
    {
      public:
        Token(std::string_view const str, Lexeme const lexeme, std::filesystem::path const& filePath,
              uint32_t const numLine);

        Token(Token const& other);

        Token(Token&& other);

        auto operator=(Token const& other) -> Token&;

        auto operator=(Token&& other) -> Token&;

        auto getLexeme() const -> Lexeme;

        auto getContent() const -> std::string_view;

        auto getNumLine() const -> uint32_t;

        auto getFilePath() const -> std::filesystem::path const&;

      private:
        std::string_view str;
        Lexeme lexeme;
        std::filesystem::path filePath;
        uint32_t numLine;
    };

    class Lexer
    {
      public:
        Lexer(std::filesystem::path const& filePath);

        Lexer(std::string_view const source, std::filesystem::path const& filePath);

        auto getTokens() const -> std::span<Token const>;

      private:
        std::vector<Token> tokens;
        std::locale locale;

        auto analyzeBufferData(std::string_view const buffer, std::filesystem::path const& filePath) -> void;

        auto isLetter(char const c) const -> bool;

        auto isNumeric(char const c) const -> bool;

        auto isType(std::string_view const str) const -> bool;

        auto isShaderCode(std::string_view const str) const -> bool;
    };
} // namespace ionengine::shadersys