// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "ast.hpp"

namespace ionengine::tools::shaderc
{
    ASTStringLiteral::ASTStringLiteral(std::string_view const value) : value(value)
    {
    }

    ASTFloatLiteral::ASTFloatLiteral(float const value) : value(value)
    {
    }

    ASTAttribute::ASTAttribute(std::string_view const attribute, std::unique_ptr<ASTNode> value)
        : attribute(attribute), value(std::move(value))
    {
    }

    ASTVariable::ASTVariable(std::string_view const identifier, std::string_view const variableType,
                             std::vector<std::unique_ptr<ASTAttribute>>&& attributes)
        : identifier(identifier), variableType(variableType), attributes(std::move(attributes))
    {
    }

    ASTStructure::ASTStructure(std::string_view const identifier, std::vector<std::unique_ptr<ASTVariable>>&& members)
        : identifier(identifier), members(std::move(members))
    {
    }

    ASTModule::ASTModule()
    {
    }

    auto ASTModule::addChildren(std::unique_ptr<ASTNode>&& children) -> ASTNode&
    {
        return *childrens.emplace_back(std::move(children));
    }
} // namespace ionengine::tools::shaderc