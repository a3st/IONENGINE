// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#undef EOF

namespace ionengine::shadersys
{
    /*!
      \brief Lexemes that can parsed from source
    */
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
        std::string str;
        Lexeme lexeme;
        std::filesystem::path filePath;
        uint32_t numLine;
    };

    enum class LexerError
    {
        EOF
    };

    /*!
      \brief Lexer that can divide source into lexems
    */
    class Lexer
    {
      public:
        Lexer();

        auto parse(std::filesystem::path const& filePath, std::string& errors)
            -> std::expected<std::vector<Token>, LexerError>;

      private:
        std::locale locale;

        auto analyzeBufferData(std::string_view const buffer, std::filesystem::path const& filePath,
                               std::vector<Token>& tokens) -> void;

        auto isLetter(char const c) const -> bool;

        auto isNumeric(char const c) const -> bool;

        auto isType(std::string_view const str) const -> bool;

        auto isShaderCode(std::string_view const str) const -> bool;
    };
} // namespace ionengine::shadersys