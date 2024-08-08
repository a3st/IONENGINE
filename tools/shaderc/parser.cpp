// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "parser.hpp"
#include "core/string.hpp"
#include "precompiled.h"

namespace ionengine::tools::shaderc
{
    Parser::Parser(std::span<Token const> const tokens)
    {
        auto it = tokens.begin();

        while (it != tokens.end())
        {
            it = this->parseModule(it);
        }
    }

    auto Parser::parseImportExpr(std::span<Token const>::iterator it) -> std::span<Token const>::iterator
    {
        if (it->getLexeme() != Lexeme::LeftParen)
        {
            throw std::runtime_error("1");
        }

        it = std::next(it);

        while (it->getLexeme() == Lexeme::Comma || it->getLexeme() != Lexeme::RightParen)
        {
            if (it->getLexeme() != Lexeme::StringLiteral)
            {
                throw std::runtime_error("1");
            }

            parseModules.emplace(it->getContent());

            std::cout << "Parsed module " << it->getContent() << std::endl;

            it = std::next(it);
        }
        return it;
    }

    auto Parser::parseAttrExpr(std::span<Token const>::iterator it) -> std::span<Token const>::iterator
    {
        if (it->getLexeme() != Lexeme::Attribute)
        {
            throw std::runtime_error("15");
        }

        std::cout << it->getContent() << std::endl;
        it = std::next(it);

        if (it->getLexeme() != Lexeme::LeftParen)
        {
            throw std::runtime_error("14");
        }

        it = std::next(it);

        if (it->getLexeme() != Lexeme::Identifier && it->getLexeme() != Lexeme::FloatLiteral)
        {
            throw std::runtime_error("12");
        }

        std::cout << it->getContent() << std::endl;
        it = std::next(it);

        if (it->getLexeme() != Lexeme::RightParen)
        {
            throw std::runtime_error("13");
        }

        return std::next(it);
    }

    auto Parser::parseModule(std::span<Token const>::iterator it) -> std::span<Token const>::iterator
    {
        switch (it->getLexeme())
        {
            case Lexeme::Keyword: {
                if (it->getContent().compare("import") == 0)
                {
                    it = std::next(it);

                    if (it->getLexeme() != Lexeme::Assignment)
                    {
                        throw std::runtime_error("1");
                    }

                    it = std::next(it);
                    this->parseImportExpr(it);
                }
                break;
            }

            case Lexeme::Identifier: {
                std::cout << it->getContent() << std::endl;
                it = std::next(it);

                if (it->getLexeme() != Lexeme::Colon)
                {
                    throw std::runtime_error("1");
                }

                it = std::next(it);

                if (it->getLexeme() == Lexeme::LeftParen)
                {
                }
                else if (it->getLexeme() == Lexeme::FixedType)
                {
                    std::string_view const variableType = it->getContent();

                    it = std::next(it);

                    while (it->getLexeme() != Lexeme::Semicolon)
                    {
                        it = this->parseAttrExpr(it);
                    }
                }
                else if (it->getLexeme() == Lexeme::Keyword)
                {
                    std::cout << it->getContent() << std::endl;
                }
                else
                {
                    throw std::runtime_error("156");
                }
            }
        }
        return std::next(it);
    }
} // namespace ionengine::tools::shaderc