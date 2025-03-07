// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/error.hpp"
#include "fx.hpp"
#include "lexer.hpp"

namespace ionengine::shadersys
{
    /*!
        \brief Data that got when parsed a shader file
    */
    struct ShaderParseData
    {
        asset::fx::HeaderData headerData;
        std::unordered_map<asset::fx::StageType, std::string> codeData;
        asset::fx::StructureData effectData;
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

        auto parse(std::span<Token const> const tokens) -> std::expected<ShaderParseData, core::error>;

      private:
        std::string errors;
        std::span<Token const> tokens;

        auto parseToken(std::span<Token const>::iterator it, ShaderParseData& parseData) -> bool;

        auto parseHeaderGroup(std::span<Token const>::iterator it, bool& successful, ShaderParseData& parseData)
            -> std::span<Token const>::iterator;

        auto parseDataGroup(std::span<Token const>::iterator it, bool& successful, ShaderParseData& parseData)
            -> std::span<Token const>::iterator;

        auto parseAttributes(std::span<Token const>::iterator it, bool& successful, ShaderParseData& parseData)
            -> std::span<Token const>::iterator;

        auto parseShaderCode(std::span<Token const>::iterator it, bool& successful, ShaderParseData& parseData)
            -> std::span<Token const>::iterator;

        auto parseOptionValue(std::span<Token const>::iterator it, bool& successful, std::string& outName,
                              std::string& outValue) -> std::span<Token const>::iterator;

        auto parseStructVariable(std::span<Token const>::iterator it, bool& successful,
                                 asset::fx::StructureElementData& outVariable) -> std::span<Token const>::iterator;
    };
} // namespace ionengine::shadersys