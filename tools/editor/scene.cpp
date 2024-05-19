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
                   << std::to_string(std::get<1>(node->getPosition())) << "],\"isExpand\":" << node->isExpanded()
                   << ",\"inputs\":[";

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
                   << ",\"sourceNode\":" << connection->getSourceNode()->getNodeID()
                   << ",\"destNode\":" << connection->getDestNode()->getNodeID()
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
        auto connection = core::make_ref<Connection>(allocIDIndex, sourceNode, sourceIndex, destNode, destIndex);
        connections.emplace_back(connection);

        connectionsOfNodes.try_emplace(sourceNode->getNodeID());
        connectionsOfNodes[sourceNode->getNodeID()].emplace_back(connections.size() - 1);

        allocIDIndex++;
        return connection;
    }

    auto Scene::dfs() -> void
    {
        std::stack<core::ref_ptr<Node>> remainingNodes;
        std::unordered_map<uint64_t, std::string> computeShaderCodes;
        std::set<uint64_t> initialFlags;
        std::stringstream initialShaderCode;
        std::stringstream resourceShaderCode;

        for (auto const& node : nodes)
        {
            computeShaderCodes[node->getNodeID()] = node->generateComputeShaderCode();

            auto result = connectionsOfNodes.find(node->getNodeID());
            if (result == connectionsOfNodes.end())
            {
                continue;
            }

            if (initialFlags.find(node->getNodeID()) == initialFlags.end())
            {
                initialFlags.emplace(node->getNodeID());
                std::string const shaderCode = node->generateInitialShaderCode();
                if (!shaderCode.empty())
                {
                    initialShaderCode << shaderCode << std::endl;
                }
            }
            std::string const shaderCode = node->generateResourceShaderCode();
            if (!shaderCode.empty())
            {
                resourceShaderCode << shaderCode << std::endl;
            }

            remainingNodes.emplace(node);
        }

        while (!remainingNodes.empty())
        {
            auto node = remainingNodes.top();
            remainingNodes.pop();

            // std::cout << node->getNodeName() << std::endl;

            for (auto const index : connectionsOfNodes[node->getNodeID()])
            {
                uint64_t offset = 0;
                bool isFailed = false;
                while (offset != std::string::npos)
                {
                    offset = computeShaderCodes[node->getNodeID()].find("##", offset);
                    if (offset == std::string::npos)
                    {
                        break;
                    }

                    uint64_t const expressionBeginOffset = offset + 2;
                    uint64_t const expressionEndOffset =
                        computeShaderCodes[node->getNodeID()].find("##", expressionBeginOffset);

                    std::string const expressionName(
                        computeShaderCodes[node->getNodeID()].begin() + expressionBeginOffset,
                        computeShaderCodes[node->getNodeID()].begin() + expressionEndOffset);

                    for (auto const& input : node->getInputSockets())
                    {
                        if (expressionName.compare(input.socketName) == 0)
                        {
                            isFailed = true;
                            break;
                        }
                    }

                    offset = expressionEndOffset + 1;
                }

                if (!isFailed)
                {
                    offset = 0;
                    while (offset != std::string::npos)
                    {
                        offset = computeShaderCodes[connections[index]->getDestNode()->getNodeID()].find("##", offset);
                        if (offset == std::string::npos)
                        {
                            break;
                        }

                        uint64_t const expressionBeginOffset = offset + 2;
                        uint64_t const expressionEndOffset =
                            computeShaderCodes[connections[index]->getDestNode()->getNodeID()].find(
                                "##", expressionBeginOffset);

                        std::string const expressionName(
                            computeShaderCodes[connections[index]->getDestNode()->getNodeID()].begin() +
                                expressionBeginOffset,
                            computeShaderCodes[connections[index]->getDestNode()->getNodeID()].begin() +
                                expressionEndOffset);

                        auto beginShaderCode =
                            computeShaderCodes[connections[index]->getDestNode()->getNodeID()].substr(0, offset);
                        auto endShaderCode = computeShaderCodes[connections[index]->getDestNode()->getNodeID()].substr(
                            expressionEndOffset + 2, std::string::npos);
                        computeShaderCodes[connections[index]->getDestNode()->getNodeID()] =
                            beginShaderCode + computeShaderCodes[node->getNodeID()] + endShaderCode;

                        offset = expressionEndOffset + 1 + computeShaderCodes[node->getNodeID()].size();
                    }

                    remainingNodes.emplace(connections[index]->getDestNode());
                }
            }
        }

        auto retNode = std::find_if(nodes.begin(), nodes.end(),
                                    [](auto const& element) { return element->getNodeName() == "Output Image"; });

        std::stringstream stream;
        stream << initialShaderCode.str() << resourceShaderCode.str()
               << computeShaderCodes[retNode->get()->getNodeID()];

        std::cout << stream.str() << std::endl;
    }
} // namespace ionengine::tools::editor