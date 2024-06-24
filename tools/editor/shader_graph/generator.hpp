// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "graph/scene.hpp"

namespace ionengine::tools::editor
{
    struct ShaderGraphComputeResult : public core::ref_counted_object
    {
        std::string outputShaderCode;
    };

    template <>
    struct GraphCompute<ShaderGraphComputeResult>
    {
        auto operator()(Scene const& scene) -> core::ref_ptr<ShaderGraphComputeResult>
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
                        else
                        {
                            offset = offset + 1;
                        }
                    }
                    out << shaderCode;
                }
            };

            for (auto const& node : scene.nodes)
            {
                computeShaderCodes[node->nodeID] =
                    scene.componentRegistry->getComponents().at(node->componentID)->generateComputeShaderCode(*node);

                auto result = scene.connectionsOfNodes.find(node->nodeID);
                if (result == scene.connectionsOfNodes.end())
                {
                    continue;
                }

                tryResolveShaderExpressions(
                    scene.componentRegistry->getComponents().at(node->componentID)->generateResourceShaderCode(*node),
                    *node, resourceShaderCode);
                remainingNodes.emplace(node);
            }

            while (!remainingNodes.empty())
            {
                auto node = remainingNodes.top();
                remainingNodes.pop();

                if (initialFlags.find(node->nodeID) == initialFlags.end())
                {
                    initialFlags.emplace(node->nodeID);

                    tryResolveShaderExpressions(scene.componentRegistry->getComponents()
                                                    .at(node->componentID)
                                                    ->generateInitialShaderCode(*node),
                                                *node, initialShaderCode);
                }

                auto foundConnection = scene.connectionsOfNodes.find(node->nodeID);
                if (foundConnection == scene.connectionsOfNodes.end())
                {
                    continue;
                }

                for (auto const index : foundConnection->second)
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

                        std::string const expressionName(
                            computeShaderCodes[node->nodeID].begin() + expressionBeginOffset,
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

                            std::string const socketName =
                                node->outputs[scene.connections[index]->sourceIndex].socketName;

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

                            std::string socketName = node->outputs[scene.connections[index]->sourceIndex].socketName;
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
                        auto nextNode = scene.connections[index]->destNode;

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

                            auto result = nextNode->options.find(expressionName);
                            if (nextNode->inputs[scene.connections[index]->destIndex].socketName.compare(
                                    expressionName) == 0)
                            {
                                auto beginShaderCode = computeShaderCodes[nextNode->nodeID].substr(0, offset);
                                auto endShaderCode = computeShaderCodes[nextNode->nodeID].substr(
                                    expressionEndOffset + 2, std::string::npos);
                                computeShaderCodes[nextNode->nodeID] =
                                    beginShaderCode + computeShaderCodes[node->nodeID] + endShaderCode;

                                offset = offset + 1 + computeShaderCodes[node->nodeID].size();
                            }
                            else if (result != nextNode->options.end())
                            {
                                auto beginShaderCode = computeShaderCodes[nextNode->nodeID].substr(0, offset);
                                auto endShaderCode = computeShaderCodes[nextNode->nodeID].substr(
                                    expressionEndOffset + 2, std::string::npos);
                                computeShaderCodes[nextNode->nodeID] = beginShaderCode + result->second + endShaderCode;

                                offset = offset + 1 + result->second.size();
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

            auto outputNode = std::find_if(scene.nodes.begin(), scene.nodes.end(),
                                           [](auto const& element) { return element->nodeName == "Output Node"; });

            std::stringstream stream;
            stream << initialShaderCode.str() << "\n"
                   << resourceShaderCode.str() << "\n"
                   << computeShaderCodes[outputNode->get()->nodeID];

            ShaderGraphComputeResult result;
            result.outputShaderCode = stream.str();
            return core::make_ref<ShaderGraphComputeResult>(result);
        }
    };
} // namespace ionengine::tools::editor