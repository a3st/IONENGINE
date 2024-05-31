// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "node.hpp"

namespace ionengine::tools::editor
{
    class Merger
    {
      public:
        Merger() = default;

        template <typename Type>
        auto registerComponent(std::string_view const domainType) -> void
        {
            auto node = core::make_ref<Type>();
            addComponentToMerger(domainType, node);
        }

        auto dump() -> std::string;

      private:
        std::unordered_map<std::string, core::ref_ptr<Node>> domains;

        auto addComponentToMerger(std::string_view const domainType, core::ref_ptr<Node> node) -> void;
    };
} // namespace ionengine::tools::editor