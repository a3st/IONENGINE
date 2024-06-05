// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "scene.hpp"
#include "precompiled.h"

namespace ionengine::tools::editor
{
    Scene::Scene(ComponentRegistry& componentRegistry) : allocIDIndex(0), componentRegistry(&componentRegistry)
    {
    }

    auto Scene::loadFromSource(std::string_view const source) -> bool
    {
        return true;
    }

    auto Scene::addNodeToScene(core::ref_ptr<Node> node) -> void
    {
        nodes.emplace_back(node);
    }

    auto Scene::dump() -> std::string
    {
        std::stringstream stream;
        stream << "{\"nodes\":[";

        bool isFirst = true;
        for (auto const& node : nodes)
        {
            if (!isFirst)
            {
                stream << ",";
            }

            stream << node->dump();
            isFirst = false;
        }
        stream << "],\"connections\":[";

        isFirst = true;
        for (auto const& connection : connections)
        {
            if (!isFirst)
            {
                stream << ",";
            }

            stream << "{\"id\":" << connection->connectionID << ",\"source\":" << connection->sourceNode->nodeID
                   << ",\"dest\":" << connection->destNode->nodeID << ",\"out\":" << connection->sourceIndex
                   << ",\"in\":" << connection->destIndex << "}";

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

        connectionsOfNodes.try_emplace(sourceNode->nodeID);
        connectionsOfNodes[sourceNode->nodeID].emplace_back(connections.size() - 1);

        allocIDIndex++;
        return connection;
    }

    auto Scene::dfs() -> std::string
    {
        std::stack<core::ref_ptr<Node>> remainingNodes;
        std::unordered_map<uint64_t, std::string> computeShaderCodes;
        std::set<uint64_t> initialFlags;
        std::stringstream initialShaderCode;
        std::stringstream resourceShaderCode;

        for (auto const& node : nodes)
        {
            computeShaderCodes[node->nodeID] =
                componentRegistry->getComponents().at(node->componentID)->generateComputeShaderCode(*node);

            auto result = connectionsOfNodes.find(node->nodeID);
            if (result == connectionsOfNodes.end())
            {
                continue;
            }

            std::string shaderCode =
                componentRegistry->getComponents().at(node->componentID)->generateResourceShaderCode(*node);
            if (!shaderCode.empty())
            {
                uint64_t offset = 0;
                while (offset != std::string::npos)
                {
                    offset = shaderCode.find("##", offset);
                    if (offset == std::string::npos)
                    {
                        break;
                    }

                    uint64_t const expressionBeginOffset = offset + 2;
                    uint64_t const expressionEndOffset = shaderCode.find("##", expressionBeginOffset);

                    std::string const expressionName(shaderCode.begin() + expressionBeginOffset,
                                                     shaderCode.begin() + expressionEndOffset);

                    auto result = node->options.find(expressionName);
                    if (result != node->options.end())
                    {
                        auto beginShaderCode = shaderCode.substr(0, offset);
                        auto endShaderCode = shaderCode.substr(expressionEndOffset + 2, std::string::npos);
                        shaderCode = beginShaderCode + result->second + endShaderCode;
                    }
                }
                resourceShaderCode << shaderCode;
            }

            remainingNodes.emplace(node);
        }

        while (!remainingNodes.empty())
        {
            auto node = remainingNodes.top();
            remainingNodes.pop();

            if (initialFlags.find(node->nodeID) == initialFlags.end())
            {
                initialFlags.emplace(node->nodeID);

                std::string shaderCode =
                    componentRegistry->getComponents().at(node->componentID)->generateInitialShaderCode(*node);
                if (!shaderCode.empty())
                {
                    uint64_t offset = 0;
                    while (offset != std::string::npos)
                    {
                        offset = shaderCode.find("##", offset);
                        if (offset == std::string::npos)
                        {
                            break;
                        }

                        uint64_t const expressionBeginOffset = offset + 2;
                        uint64_t const expressionEndOffset = shaderCode.find("##", expressionBeginOffset);

                        std::string const expressionName(shaderCode.begin() + expressionBeginOffset,
                                                         shaderCode.begin() + expressionEndOffset);

                        auto result = node->options.find(expressionName);
                        if (result != node->options.end())
                        {
                            auto beginShaderCode = shaderCode.substr(0, offset);
                            auto endShaderCode = shaderCode.substr(expressionEndOffset + 2, std::string::npos);
                            shaderCode = beginShaderCode + result->second + endShaderCode;
                        }
                    }
                    initialShaderCode << shaderCode;
                }
            }

            for (auto const index : connectionsOfNodes[node->nodeID])
            {
                bool isConnectionExpr = false;
                uint64_t offset = 0;
                bool isFailed = false;
                while (offset != std::string::npos)
                {
                    offset = computeShaderCodes[node->nodeID].find("##", offset);
                    if (offset == std::string::npos)
                    {
                        break;
                    }

                    uint64_t const expressionBeginOffset = offset + 2;
                    uint64_t const expressionEndOffset =
                        computeShaderCodes[node->nodeID].find("##", expressionBeginOffset);

                    std::string const expressionName(computeShaderCodes[node->nodeID].begin() + expressionBeginOffset,
                                                     computeShaderCodes[node->nodeID].begin() + expressionEndOffset);

                    auto result = node->options.find(expressionName);
                    if (result != node->options.end())
                    {
                        auto beginShaderCode = computeShaderCodes[node->nodeID].substr(0, offset);
                        auto endShaderCode =
                            computeShaderCodes[node->nodeID].substr(expressionEndOffset + 2, std::string::npos);
                        computeShaderCodes[node->nodeID] = beginShaderCode + result->second + endShaderCode;
                    }
                    else if (expressionName.compare("__CONNECTION__") == 0)
                    {
                        isConnectionExpr = true;
                    }
                    else
                    {
                        for (auto const& input : node->inputs)
                        {
                            if (expressionName.compare(input.socketName) == 0)
                            {
                                isFailed = true;
                                break;
                            }
                        }

                        offset = expressionEndOffset + 1;
                    }
                }

                if (!isFailed)
                {
                    offset = 0;
                    while (offset != std::string::npos)
                    {
                        offset = computeShaderCodes[connections[index]->destNode->nodeID].find("##", offset);
                        if (offset == std::string::npos)
                        {
                            break;
                        }

                        uint64_t const expressionBeginOffset = offset + 2;
                        uint64_t const expressionEndOffset =
                            computeShaderCodes[connections[index]->destNode->nodeID].find("##", expressionBeginOffset);

                        std::string const expressionName(
                            computeShaderCodes[connections[index]->destNode->nodeID].begin() + expressionBeginOffset,
                            computeShaderCodes[connections[index]->destNode->nodeID].begin() + expressionEndOffset);

                        auto beginShaderCode =
                            computeShaderCodes[connections[index]->destNode->nodeID].substr(0, offset);
                        auto endShaderCode = computeShaderCodes[connections[index]->destNode->nodeID].substr(
                            expressionEndOffset + 2, std::string::npos);
                        computeShaderCodes[connections[index]->destNode->nodeID] =
                            beginShaderCode + computeShaderCodes[node->nodeID] + endShaderCode;

                        offset = expressionEndOffset + 1 + computeShaderCodes[node->nodeID].size();
                    }

                    remainingNodes.emplace(connections[index]->destNode);
                }

                if (isConnectionExpr)
                {
                    computeShaderCodes[node->nodeID] =
                        componentRegistry->getComponents().at(node->componentID)->generateComputeShaderCode(*node);
                }
            }
        }

        auto retNode = std::find_if(nodes.begin(), nodes.end(),
                                    [](auto const& element) { return element->nodeName == "Output Image"; });

        std::stringstream stream;
        stream << initialShaderCode.str() << resourceShaderCode.str() << computeShaderCodes[retNode->get()->nodeID];

        return stream.str();
    }
} // namespace ionengine::tools::editor