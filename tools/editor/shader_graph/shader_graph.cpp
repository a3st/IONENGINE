// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "shader_graph.hpp"
#include "core/exception.hpp"
#include "graph/registry.hpp"
#include "precompiled.h"

#include "shader_graph/components/input.hpp"
#include "shader_graph/components/output.hpp"

namespace ionengine::tools::editor
{
    ShaderGraphAsset::ShaderGraphAsset(ComponentRegistry& componentRegistry) : componentRegistry(&componentRegistry)
    {
    }

    auto ShaderGraphAsset::create(ShaderGraphType const graphType) -> void
    {
        this->graphData.graphType = graphType;

        auto createNode = [&]<typename Type>(uint64_t const nodeID, int32_t const posX, int32_t const posY) {
            auto const nodeComponent = componentRegistry->getComponentByType<Type>();

            NodeData nodeData = {.nodeID = nodeID,
                                 .nodeName = std::string(nodeComponent->getName()),
                                 .nodePosition = {posX, posY},
                                 .nodeFixed = nodeComponent->isFixed(),
                                 .nodeUserData = {.nodeComponentID = nodeComponent->componentID,
                                                  .nodeOptions = nodeComponent->setOptions()}};

            for (auto const& input : nodeComponent->setInputs())
            {
                NodeSocketData socketData = {.socketName = input.socketName, .socketType = input.socketType};
                nodeData.nodeInputs.emplace_back(std::move(socketData));
            }

            for (auto const& output : nodeComponent->setOutputs())
            {
                NodeSocketData socketData = {.socketName = output.socketName, .socketType = output.socketType};
                nodeData.nodeOutputs.emplace_back(std::move(socketData));
            }

            this->graphData.sceneData.nodes.emplace_back(std::move(nodeData));
        };

        if (graphType == ShaderGraphType::Unlit)
        {
            createNode.template operator()<UnlitOutput_NodeComponent>(0, 600, 80);
        }

        createNode.template operator()<Input_NodeComponent>(1, 10, 80);
    }

    auto ShaderGraphAsset::loadFromFile(std::filesystem::path const& filePath) -> bool
    {
        auto result = core::loadFromFile<ShaderGraphFile>(filePath);
        if (!result.has_value())
        {
            return false;
        }

        ShaderGraphFile shaderGraphFile = std::move(result.value());
        this->graphData = shaderGraphFile.graphData;
        return true;
    }

    auto ShaderGraphAsset::loadFromBytes(std::span<uint8_t const> const dataBytes) -> bool
    {
        auto result = core::loadFromBytes<ShaderGraphFile>(dataBytes);
        if (!result.has_value())
        {
            return false;
        }

        ShaderGraphFile shaderGraphFile = std::move(result.value());
        this->graphData = shaderGraphFile.graphData;
        return true;
    }

    auto ShaderGraphAsset::getGraphData() const -> ShaderGraphData const&
    {
        return graphData;
    }
} // namespace ionengine::tools::editor