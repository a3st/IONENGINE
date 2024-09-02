// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "ast.hpp"
#include "core/error.hpp"
#include "lexer.hpp"

namespace ionengine::tools::shaderc
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
        Parser(Lexer const& lexer, std::filesystem::path const& basePath);

      private:
        std::set<std::string> parseModules;
        std::unique_ptr<ASTModule> shaderModule;
        std::filesystem::path basePath;

        std::set<std::string> identifierCache;

        auto parseImportExpr(std::span<Token const>::iterator it) -> std::span<Token const>::iterator;

        auto parseModule(std::span<Token const>::iterator it,
                         std::unique_ptr<ASTModule>& module) -> std::span<Token const>::iterator;

        auto parseAttrExpr(std::span<Token const>::iterator it,
                           std::unique_ptr<ASTAttribute>& attribute) -> std::span<Token const>::iterator;
    };
} // namespace ionengine::tools::shaderc