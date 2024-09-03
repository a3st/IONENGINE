// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::rhi::fx
{
    class ASTNode
    {
      public:
        virtual ~ASTNode() = default;
    };

    class ASTStringLiteral : public ASTNode
    {
      public:
        ASTStringLiteral(std::string_view const value);

      private:
        std::string value;
    };

    class ASTFloatLiteral : public ASTNode
    {
      public:
        ASTFloatLiteral(float const value);

      private:
        float value;
    };

    class ASTAttribute : public ASTNode
    {
      public:
        ASTAttribute(std::string_view const attribute, std::unique_ptr<ASTNode> value);

      private:
        std::string attribute;
        std::unique_ptr<ASTNode> value;
    };

    class ASTVariable : public ASTNode
    {
      public:
        ASTVariable(std::string_view const identifier, std::string_view const variableType,
                    std::vector<std::unique_ptr<ASTAttribute>>&& attributes);

      private:
        std::string identifier;
        std::string variableType;
        std::vector<std::unique_ptr<ASTAttribute>> attributes;
    };

    class ASTStructure : public ASTNode
    {
      public:
        ASTStructure(std::string_view const identifier, std::vector<std::unique_ptr<ASTVariable>>&& members);

      private:
        std::string identifier;
        std::vector<std::unique_ptr<ASTVariable>> members;
    };

    class ASTModule : public ASTNode
    {
      public:
        ASTModule();

        auto addChildren(std::unique_ptr<ASTNode>&& children) -> ASTNode&;

      private:
        std::vector<std::unique_ptr<ASTNode>> childrens;
    };
} // namespace ionengine::tools::shaderc