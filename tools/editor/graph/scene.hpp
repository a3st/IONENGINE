// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "graph/connection.hpp"
#include "graph/node.hpp"
#include "graph/registry.hpp"

namespace ionengine::tools::editor
{
    class Scene : public core::ref_counted_object
    {
      public:
        Scene(ComponentRegistry& componentRegistry);

        template <typename Type>
        auto createNodeByType(uint64_t const nodeID) -> core::ref_ptr<Node>
        {
            auto createdNode = componentRegistry->getComponentByType<Type>()->create(nodeID);
            addNodeToScene(createdNode);
            return createdNode;
        }

        auto createNodeByID(uint32_t const componentID, uint64_t const nodeID) -> core::ref_ptr<Node>;

        auto createConnection(uint64_t const connectionID, core::ref_ptr<Node> sourceNode, uint32_t const sourceIndex,
                              core::ref_ptr<Node> destNode, uint32_t const destIndex) -> core::ref_ptr<Connection>;

        auto bfs(std::stringstream& shaderCodeStream) -> void;

      private:
        ComponentRegistry* componentRegistry;

        std::vector<core::ref_ptr<Node>> nodes;
        std::vector<core::ref_ptr<Connection>> connections;
        std::unordered_map<uint64_t, std::vector<size_t>> connectionsOfNodes;

        auto addNodeToScene(core::ref_ptr<Node> node) -> void;
    };
} // namespace ionengine::tools::editor