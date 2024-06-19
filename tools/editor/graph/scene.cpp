// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "graph/scene.hpp"
#include "precompiled.h"

namespace ionengine::tools::editor
{
    Scene::Scene(ComponentRegistry& componentRegistry) : componentRegistry(&componentRegistry)
    {
    }

    auto Scene::addNodeToScene(core::ref_ptr<Node> node) -> void
    {
        nodes.emplace_back(node);
    }

    auto Scene::createNodeByID(uint32_t const componentID, uint64_t const nodeID) -> core::ref_ptr<Node>
    {
        auto createdNode = componentRegistry->getComponents().at(componentID)->create(nodeID);
        this->addNodeToScene(createdNode);
        return createdNode;
    }

    auto Scene::createConnection(uint64_t const connectionID, core::ref_ptr<Node> sourceNode,
                                 uint32_t const sourceIndex, core::ref_ptr<Node> destNode,
                                 uint32_t const destIndex) -> core::ref_ptr<Connection>
    {
        auto createdConnection = core::make_ref<Connection>(connectionID, sourceNode, sourceIndex, destNode, destIndex);
        connections.emplace_back(createdConnection);

        connectionsOfNodes.try_emplace(sourceNode->nodeID);
        connectionsOfNodes[sourceNode->nodeID].emplace_back(connections.size() - 1);
        return createdConnection;
    }

    auto Scene::getNodes() const -> std::span<core::ref_ptr<Node> const>
    {
        return nodes;
    }

    auto Scene::getConnections() const -> std::span<core::ref_ptr<Connection> const>
    {
        return connections;
    }
} // namespace ionengine::tools::editor