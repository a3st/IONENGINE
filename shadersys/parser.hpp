// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/error.hpp"
#include "fx.hpp"
#include "lexer.hpp"

namespace ionengine::shadersys
{
    class parser_error : public core::runtime_error
    {
      public:
        parser_error(std::string_view const error) : core::runtime_error(error)
        {
        }
    };

    class Parser
    {
      public:
        Parser() = default;

        auto parse(Lexer const& lexer, fx::ShaderHeaderData& headerData, fx::ShaderOutputData& outputData) -> bool;

      private:
        auto parseOptionValue(std::span<Token const>::iterator variable, std::span<Token const>::iterator it,
                              std::string& value) -> std::span<Token const>::iterator;

        auto parseOutputBlockExpr(std::span<Token const>::iterator it) -> std::span<Token const>::iterator;
    };
} // namespace ionengine::shadersys