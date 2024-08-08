// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "lexer.hpp"

namespace ionengine::tools::shaderc
{
    class Parser
    {
      public:
        Parser(std::span<Token const> const tokens);

      private:
        std::set<std::string> parseModules;

        auto parseImportExpr(std::span<Token const>::iterator it) -> std::span<Token const>::iterator;

        auto parseModule(std::span<Token const>::iterator it) -> std::span<Token const>::iterator;

        auto parseAttrExpr(std::span<Token const>::iterator it) -> std::span<Token const>::iterator;
    };
} // namespace ionengine::tools::shaderc