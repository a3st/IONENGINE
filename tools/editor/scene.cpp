// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "scene.hpp"
#include "precompiled.h"

namespace ionengine::tools::editor
{
    Scene::Scene() : allocIDIndex(0)
    {
    }

    auto Scene::addNodeToScene(core::ref_ptr<Node> node) -> void
    {
        nodes.emplace_back(node);
    }

    auto Scene::dump() -> std::string
    {
        std::stringstream stream;
        stream << "{\"sceneName\":" << "\"Test\"," << "\"sceneNodes\":[";

        bool isFirst = true;
        for (auto const& node : nodes)
        {
            if (!isFirst)
            {
                stream << ",";
            }

            stream << "{\"nodeID\":" << node->getNodeID() << ",\"nodeName\":\"" << node->getNodeName()
                   << "\",\"position\":[" << std::to_string(std::get<0>(node->getPosition())) << ","
                   << std::to_string(std::get<1>(node->getPosition())) << "],\"inputs\":[";

            isFirst = true;
            for (auto const& socket : node->getInputSockets())
            {
                if (!isFirst)
                {
                    stream << ",";
                }

                stream << "{\"socketName\":\"" << socket.socketName << "\",\"socketType\":\"" << socket.socketType
                       << "\"}";

                isFirst = false;
            }

            stream << "],\"outputs\":[";

            isFirst = true;
            for (auto const& socket : node->getOutputSockets())
            {
                if (!isFirst)
                {
                    stream << ",";
                }

                stream << "{\"socketName\":\"" << socket.socketName << "\",\"socketType\":\"" << socket.socketType
                       << "\"}";

                isFirst = false;
            }

            stream << "]}";

            isFirst = false;
        }
        stream << "],\"sceneConnections\":[";

        isFirst = true;
        for (auto const& connection : connections)
        {
            if (!isFirst)
            {
                stream << ",";
            }

            stream << "{\"connectionID\":" << connection->getConnectionID()
                   << ",\"sourceNode\":" << connection->getSourceNodeID()
                   << ",\"destNode\":" << connection->getDestNodeID()
                   << ",\"sourceIndex\":" << connection->getSourceIndex()
                   << ",\"destIndex\":" << connection->getDestIndex() << "}";

            isFirst = false;
        }

        stream << "]}";
        return stream.str();
    }

    auto Scene::createConnection(core::ref_ptr<Node> sourceNode, uint64_t const sourceIndex,
                                 core::ref_ptr<Node> destNode, uint64_t const destIndex) -> core::ref_ptr<Connection>
    {
        auto connection = core::make_ref<Connection>(allocIDIndex, sourceNode->getNodeID(), sourceIndex,
                                                     destNode->getNodeID(), destIndex);
        connections.emplace_back(connection);

        connectionsOfNodes.try_emplace(destNode->getNodeID());
        connectionsOfNodes[destNode->getNodeID()].emplace_back(destIndex);

        allocIDIndex++;
        return connection;
    }

    auto Scene::dfs() -> void
    {
        for (auto const& node : nodes)
        {
            auto result = connectionsOfNodes.find(node->getNodeID());
            if (result == connectionsOfNodes.end())
            {
                continue;
            }

            std::cout << node->getNodeName() << std::endl;
        }
    }
} // namespace ionengine::tools::editor