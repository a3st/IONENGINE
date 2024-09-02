// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "parser.hpp"
#include "core/string.hpp"
#include "precompiled.h"

namespace ionengine::tools::shaderc
{
    Parser::Parser(Lexer const& lexer, std::filesystem::path const& basePath) : basePath(basePath)
    {
        auto tokens = lexer.getTokens();

        auto it = tokens.begin();
        while (it != tokens.end())
        {
            it = this->parseModule(it, shaderModule);
        }
    }

    auto Parser::parseImportExpr(std::span<Token const>::iterator it) -> std::span<Token const>::iterator
    {
        if (it->getLexeme() != Lexeme::LeftParen)
        {
            throw parser_error(std::format(""));
        }

        it = std::next(it);

        while (it->getLexeme() != Lexeme::RightParen)
        {
            if (it->getLexeme() == Lexeme::Comma)
            {
                it = std::next(it);
            }

            if (it->getLexeme() != Lexeme::StringLiteral)
            {
                throw parser_error("1444");
            }

            parseModules.emplace(it->getContent());

            // Parse Module
            std::string const fileName = std::string(it->getContent()) + ".fx";
            if (!std::filesystem::exists(basePath / fileName))
            {
                throw parser_error(
                    std::format("{}: error {}: '{}' module not found", it->getFilePath(), 1, it->getContent()));
            }

            // Lexer lexer()
            std::cout << "Parsed module " << it->getContent() << std::endl;

            it = std::next(it);
        }
        return it;
    }

    auto Parser::parseAttrExpr(std::span<Token const>::iterator it,
                               std::unique_ptr<ASTAttribute>& attribute) -> std::span<Token const>::iterator
    {
        if (it->getLexeme() != Lexeme::Attribute)
        {
            throw parser_error("15");
        }

        std::string_view const attributeName = it->getContent();
        it = std::next(it);

        if (it->getLexeme() != Lexeme::LeftParen)
        {
            throw parser_error("14");
        }

        it = std::next(it);

        if (it->getLexeme() == Lexeme::StringLiteral)
        {
            auto stringLiteral = std::make_unique<ASTStringLiteral>(it->getContent());
            attribute = std::make_unique<ASTAttribute>(attributeName, std::move(stringLiteral));

            std::cout << it->getContent() << std::endl;
        }
        else if (it->getLexeme() == Lexeme::FloatLiteral)
        {
            auto floatLiteral = std::make_unique<ASTFloatLiteral>(core::ston<float>(it->getContent()));
            attribute = std::make_unique<ASTAttribute>(attributeName, std::move(floatLiteral));
        }
        else
        {
            throw parser_error("12");
        }

        it = std::next(it);

        if (it->getLexeme() != Lexeme::RightParen)
        {
            throw parser_error("13");
        }
        return std::next(it);
    }

    auto Parser::parseModule(std::span<Token const>::iterator it,
                             std::unique_ptr<ASTModule>& module) -> std::span<Token const>::iterator
    {
        switch (it->getLexeme())
        {
            case Lexeme::Keyword: {
                if (it->getContent().compare("import") == 0)
                {
                    it = std::next(it);

                    if (it->getLexeme() != Lexeme::Assignment)
                    {
                        throw parser_error("1");
                    }

                    it = std::next(it);
                    this->parseImportExpr(it);
                }
                break;
            }

            case Lexeme::Identifier: {
                std::cout << it->getContent() << std::endl;
                std::string_view const identifierName = it->getContent();

                it = std::next(it);

                if (it->getLexeme() != Lexeme::Colon)
                {
                    throw parser_error("1");
                }

                it = std::next(it);

                if (it->getLexeme() == Lexeme::LeftParen)
                {
                    std::cout << 11111 << std::endl;
                }
                else if (it->getLexeme() == Lexeme::FixedType)
                {
                    std::string_view const variableType = it->getContent();

                    if (variableType.compare("struct") == 0)
                    {
                        it = std::next(it);

                        while (it->getLexeme() != Lexeme::RightParen)
                        {
                            std::string_view const variableName = it->getContent();

                            it = std::next(it);

                            std::vector<std::unique_ptr<ASTAttribute>> attributes;

                            it = std::next(it);

                            while (it->getLexeme() != Lexeme::Semicolon)
                            {
                                std::unique_ptr<ASTAttribute> attribute;
                                it = this->parseAttrExpr(it, attribute);
                                attributes.emplace_back(std::move(attribute));
                            }

                            auto variable =
                                std::make_unique<ASTVariable>(variableName, variableType, std::move(attributes));
                        }

                        identifierCache.emplace(identifierName);
                    }
                    else
                    {
                    }
                }
                else if (it->getLexeme() == Lexeme::Keyword)
                {
                    std::cout << it->getContent() << std::endl;
                }
                else
                {
                    if (identifierCache.find(std::string(it->getContent())) == identifierCache.end())
                    {
                        throw parser_error(std::format("{}: error {}: '{}' undeclared identifier", it->getFilePath(), 2,
                                                       it->getContent()));
                    }

                    std::cout << "OK" << std::endl;
                }
            }
        }
        return std::next(it);
    }
} // namespace ionengine::tools::shaderc