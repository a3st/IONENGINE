// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "connection.hpp"
#include "node.hpp"
#include "registry.hpp"

namespace ionengine::tools::editor
{

    class Scene : public core::ref_counted_object
    {
      public:
        Scene(ComponentRegistry& componentRegistry);

        auto loadFromSource(std::string_view const source) -> bool;

        template <typename Type>
        auto createNode(uint32_t const posX, uint32_t const posY) -> core::ref_ptr<Type>
        {
            auto node = core::make_ref<Type>(allocIDIndex, posX, posY);
            addNodeToScene(node);
            allocIDIndex++;
            return node;
        }

        auto createConnection(core::ref_ptr<Node> sourceNode, uint64_t const sourceIndex, core::ref_ptr<Node> destNode,
                              uint64_t const destIndex) -> core::ref_ptr<Connection>;

        auto dfs() -> std::string;

        auto dump() -> std::string;

      private:
        ComponentRegistry* componentRegistry;

        std::vector<core::ref_ptr<Node>> nodes;
        std::vector<core::ref_ptr<Connection>> connections;
        std::unordered_map<uint64_t, std::vector<size_t>> connectionsOfNodes;
        uint64_t allocIDIndex;

        auto addNodeToScene(core::ref_ptr<Node> node) -> void;
    };
} // namespace ionengine::tools::editor