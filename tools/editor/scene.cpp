// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "scene.hpp"
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

    auto Scene::bfs(std::stringstream& shaderCodeStream) -> void
    {
        std::stack<core::ref_ptr<Node>> remainingNodes;
        std::unordered_map<uint64_t, std::string> computeShaderCodes;
        std::set<uint64_t> initialFlags;
        std::stringstream initialShaderCode;
        std::stringstream resourceShaderCode;

        auto tryResolveShaderExpressions = [](std::string shaderCode, Node const& node,
                                              std::stringstream& out) -> void {
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

                    auto result = node.options.find(expressionName);
                    if (result != node.options.end())
                    {
                        auto beginShaderCode = shaderCode.substr(0, offset);
                        auto endShaderCode = shaderCode.substr(expressionEndOffset + 2, std::string::npos);
                        shaderCode = beginShaderCode + result->second + endShaderCode;
                        offset = offset + 1 + result->second.size();
                    }
                }
                out << shaderCode;
            }
        };

        for (auto const& node : nodes)
        {
            computeShaderCodes[node->nodeID] =
                componentRegistry->getComponents().at(node->componentID)->generateComputeShaderCode(*node);

            auto result = connectionsOfNodes.find(node->nodeID);
            if (result == connectionsOfNodes.end())
            {
                continue;
            }

            tryResolveShaderExpressions(
                componentRegistry->getComponents().at(node->componentID)->generateResourceShaderCode(*node), *node,
                resourceShaderCode);
            remainingNodes.emplace(node);
        }

        while (!remainingNodes.empty())
        {
            auto node = remainingNodes.top();
            remainingNodes.pop();

            if (initialFlags.find(node->nodeID) == initialFlags.end())
            {
                initialFlags.emplace(node->nodeID);

                tryResolveShaderExpressions(
                    componentRegistry->getComponents().at(node->componentID)->generateInitialShaderCode(*node), *node,
                    initialShaderCode);
            }

            for (auto const index : connectionsOfNodes[node->nodeID])
            {
                bool isConnectionExpr = false;
                uint64_t offset = 0;
                bool isFailed = false;
                std::string tempShaderCode;
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

                        offset = offset + 1 + result->second.size();
                    }
                    else if (expressionName.compare("__CONNECTION__") == 0)
                    {
                        auto beginShaderCode = computeShaderCodes[node->nodeID].substr(0, offset);
                        auto endShaderCode =
                            computeShaderCodes[node->nodeID].substr(expressionEndOffset + 2, std::string::npos);

                        std::string const socketName = node->outputs[connections[index]->sourceIndex].socketName;

                        tempShaderCode = computeShaderCodes[node->nodeID];
                        computeShaderCodes[node->nodeID] = beginShaderCode + socketName + endShaderCode;

                        isConnectionExpr = true;
                        offset = offset + 1 + socketName.size();
                    }
                    else if (expressionName.compare("__CONNECTION_LOWER__") == 0)
                    {
                        auto beginShaderCode = computeShaderCodes[node->nodeID].substr(0, offset);
                        auto endShaderCode =
                            computeShaderCodes[node->nodeID].substr(expressionEndOffset + 2, std::string::npos);

                        std::string socketName = node->outputs[connections[index]->sourceIndex].socketName;
                        std::transform(socketName.begin(), socketName.end(), socketName.begin(),
                                       [](auto const ch) { return std::tolower(ch); });

                        tempShaderCode = computeShaderCodes[node->nodeID];
                        computeShaderCodes[node->nodeID] = beginShaderCode + socketName + endShaderCode;

                        isConnectionExpr = true;
                        offset = offset + 1 + socketName.size();
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
                    auto nextNode = connections[index]->destNode;

                    offset = 0;
                    while (offset != std::string::npos)
                    {
                        offset = computeShaderCodes[nextNode->nodeID].find("##", offset);
                        if (offset == std::string::npos)
                        {
                            break;
                        }

                        uint64_t const expressionBeginOffset = offset + 2;
                        uint64_t const expressionEndOffset =
                            computeShaderCodes[nextNode->nodeID].find("##", expressionBeginOffset);

                        std::string const expressionName(
                            computeShaderCodes[nextNode->nodeID].begin() + expressionBeginOffset,
                            computeShaderCodes[nextNode->nodeID].begin() + expressionEndOffset);

                        if (nextNode->inputs[connections[index]->destIndex].socketName.compare(expressionName) == 0)
                        {
                            auto beginShaderCode = computeShaderCodes[nextNode->nodeID].substr(0, offset);
                            auto endShaderCode =
                                computeShaderCodes[nextNode->nodeID].substr(expressionEndOffset + 2, std::string::npos);
                            computeShaderCodes[nextNode->nodeID] =
                                beginShaderCode + computeShaderCodes[node->nodeID] + endShaderCode;

                            offset = offset + 1 + computeShaderCodes[node->nodeID].size();
                        }
                        else
                        {
                            offset = expressionEndOffset + 1;
                        }
                    }

                    remainingNodes.emplace(nextNode);
                }

                if (isConnectionExpr)
                {
                    computeShaderCodes[node->nodeID] = tempShaderCode;
                }
            }
        }

        auto outputNode = std::find_if(nodes.begin(), nodes.end(),
                                       [](auto const& element) { return element->nodeName == "Output Node"; });

        shaderCodeStream << initialShaderCode.str() << std::endl
                         << resourceShaderCode.str() << std::endl
                         << computeShaderCodes[outputNode->get()->nodeID];
    }
} // namespace ionengine::tools::editor