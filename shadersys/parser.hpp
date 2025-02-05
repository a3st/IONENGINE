// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/error.hpp"
#include "core/string.hpp"
#include "fx.hpp"
#include "lexer.hpp"
#undef EOF

namespace ionengine::shadersys
{
    /*!
        \brief Errors that will get when parse a shader file
    */
    enum class ParseError : uint32_t
    {
        EOF = 1000,
        Operator = 1001,
        Character = 1002,
        InvalidType = 1003,
        ShaderCode = 1005,
        UnknownSection = 1006,
        EOS = 1007
    };

    /*!
        \brief Data that got when parsed a shader file
    */
    struct ShaderParseData
    {
        asset::fx::HeaderData headerData;
        std::unordered_map<asset::fx::StageType, std::string> codeData;
        asset::fx::StructureData materialData;
        std::unordered_map<std::string, std::string> csAttributes;
        std::unordered_map<std::string, std::string> psAttributes;
    };

    /*!
        \brief Parser that can parse a FX shader file
    */
    class Parser
    {
      public:
        Parser() = default;

        auto parse(std::span<Token const> const tokens, std::string& errors) -> std::expected<ShaderParseData, ParseError>;

      private:
        std::string* errors;
        std::span<Token const> tokens;

        auto parseToken(std::span<Token const>::iterator it, ShaderParseData& parseData) -> std::optional<ParseError>;

        auto parseHeaderGroup(std::span<Token const>::iterator it, std::optional<ParseError>& errorCode,
                              ShaderParseData& parseData) -> std::span<Token const>::iterator;

        auto parseDataGroup(std::span<Token const>::iterator it, std::optional<ParseError>& errorCode,
                            ShaderParseData& parseData) -> std::span<Token const>::iterator;

        auto parseAttributes(std::span<Token const>::iterator it, std::optional<ParseError>& errorCode,
                             ShaderParseData& parseData) -> std::span<Token const>::iterator;

        auto parseShaderCode(std::span<Token const>::iterator it, std::optional<ParseError>& errorCode,
                             ShaderParseData& parseData) -> std::span<Token const>::iterator;

        auto parseOptionValue(std::span<Token const>::iterator it, std::optional<ParseError>& errorCode,
                              std::string& outName, std::string& outValue) -> std::span<Token const>::iterator;

        auto parseStructVariable(std::span<Token const>::iterator it, std::optional<ParseError>& errorCode,
                                 asset::fx::StructureElementData& outVariable) -> std::span<Token const>::iterator;
    };
} // namespace ionengine::shadersys