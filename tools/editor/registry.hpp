// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "node.hpp"

namespace ionengine::tools::editor
{
    class DomainRegistry
    {
      public:
        DomainRegistry() = default;

        template <typename Type>
        auto registerDomain(std::string_view const domainType) -> void
        {
            auto node = core::make_ref<Type>();
            addDomainToRegistry(domainType, node);
        }

        auto dump() -> std::string;

      private:
        std::unordered_map<std::string, core::ref_ptr<Node>> domains;

        auto addDomainToRegistry(std::string_view const domainType, core::ref_ptr<Node> node) -> void;
    };

    class ComponentRegistry
    {
      public:
        ComponentRegistry() = default;

        template <typename Type>
        auto registerComponent(std::string_view const group) -> void
        {
            auto node = core::make_ref<Type>();
            addComponentToRegistry(group, node);
        }

        auto dump() -> std::string;

      private:
        std::unordered_map<std::string, std::vector<core::ref_ptr<Node>>> groups;

        auto addComponentToRegistry(std::string_view const group, core::ref_ptr<Node> node) -> void;
    };
} // namespace ionengine::tools::editor