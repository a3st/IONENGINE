// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "shader_graph.hpp"
#include "core/exception.hpp"
#include "core/subprocess.hpp"
#include "precompiled.h"

#include "shader_graph/components/convert.hpp"
#include "shader_graph/components/input.hpp"
#include "shader_graph/components/math.hpp"
#include "shader_graph/components/output.hpp"
#include "shader_graph/components/texture.hpp"

namespace ionengine::tools::editor
{
    ShaderGraphEditor::ShaderGraphEditor()
    {
        componentRegistry.registerComponent<Input_NodeComponent>();
        componentRegistry.registerComponent<UnlitOutput_NodeComponent>();
        componentRegistry.registerComponent<Sampler2D_NodeComponent>();
        componentRegistry.registerComponent<Constant_Color_NodeComponent>();
        componentRegistry.registerComponent<Constant_Float_NodeComponent>();
        componentRegistry.registerComponent<Join_Float4_NodeComponent>();
        componentRegistry.registerComponent<Split_Float4_NodeComponent>();
        componentRegistry.registerComponent<Split_Float3_NodeComponent>();
    }

    auto ShaderGraphEditor::create(ShaderGraphType const graphType) -> void
    {
        sceneGraph = core::make_ref<Scene>(componentRegistry);
        if (graphType == ShaderGraphType::Unlit)
        {
            auto createdNode = sceneGraph->createNodeByType<UnlitOutput_NodeComponent>(0);
            createdNode->posX = 600;
            createdNode->posY = 80;
        }
        this->graphType = graphType;

        auto createdNode = sceneGraph->createNodeByType<Input_NodeComponent>(1);
        createdNode->posX = 10;
        createdNode->posY = 80;
    }

    auto ShaderGraphEditor::loadFromFile(std::filesystem::path const& filePath) -> bool
    {
        auto result = core::loadFromFile<ShaderGraphFile>(filePath);
        if (!result.has_value())
        {
            return false;
        }

        ShaderGraphFile shaderGraphFile = std::move(result.value());
        graphType = shaderGraphFile.graphData.graphType;

        sceneGraph = core::make_ref<Scene>(componentRegistry);
        std::unordered_map<uint64_t, core::ref_ptr<Node>> componentsOfNodes;

        for (auto const& node : shaderGraphFile.graphData.sceneData.nodes)
        {
            auto createdNode = sceneGraph->createNodeByID(node.nodeUserData.nodeComponentID, node.nodeID);
            componentsOfNodes.emplace(node.nodeID, createdNode);

            createdNode->posX = node.nodePosition[0];
            createdNode->posY = node.nodePosition[1];

            if (!node.nodeInputs.empty())
            {
                createdNode->inputs.clear();
                for (auto const& input : node.nodeInputs)
                {
                    Node::SocketInfo socketInfo = {.socketName = input.socketName, .socketType = input.socketType};
                    createdNode->inputs.emplace_back(std::move(socketInfo));
                }
            }

            if (!node.nodeOutputs.empty())
            {
                createdNode->outputs.clear();
                for (auto const& output : node.nodeOutputs)
                {
                    Node::SocketInfo socketInfo = {.socketName = output.socketName, .socketType = output.socketType};
                    createdNode->outputs.emplace_back(std::move(socketInfo));
                }
            }

            for (auto const& [key, value] : node.nodeUserData.nodeOptions)
            {
                createdNode->options[key] = value;
            }
        }

        for (auto const& connection : shaderGraphFile.graphData.sceneData.connections)
        {
            sceneGraph->createConnection(connection.connectionID, componentsOfNodes[connection.sourceNodeID],
                                         connection.sourceIndex, componentsOfNodes[connection.destNodeID],
                                         connection.destIndex);
        }
        return true;
    }

    auto ShaderGraphEditor::getScene() -> core::ref_ptr<Scene>
    {
        return sceneGraph;
    }

    auto ShaderGraphEditor::getComponentRegistry() -> editor::ComponentRegistry&
    {
        return componentRegistry;
    }

    auto ShaderGraphEditor::dump() -> ShaderGraphData
    {
        ShaderGraphData shaderGraph = {};
        shaderGraph.graphType = graphType;

        for (auto const& node : sceneGraph->getNodes())
        {
            NodeData nodeData = {.nodeID = node->nodeID,
                                 .nodeName =
                                     std::string(componentRegistry.getComponents().at(node->componentID)->getName()),
                                 .nodePosition = {node->posX, node->posY},
                                 .nodeFixed = componentRegistry.getComponents().at(node->componentID)->isFixed(),
                                 .nodeUserData = {.nodeComponentID = node->componentID, .nodeOptions = node->options}};

            for (auto const& input : node->inputs)
            {
                NodeSocketData socketData = {.socketName = input.socketName, .socketType = input.socketType};
                nodeData.nodeInputs.emplace_back(std::move(socketData));
            }

            for (auto const& output : node->outputs)
            {
                NodeSocketData socketData = {.socketName = output.socketName, .socketType = output.socketType};
                nodeData.nodeOutputs.emplace_back(std::move(socketData));
            }

            shaderGraph.sceneData.nodes.emplace_back(std::move(nodeData));
        }

        for (auto const& connection : sceneGraph->getConnections())
        {
            ConnectionData connectionData = {.connectionID = connection->connectionID,
                                             .sourceNodeID = connection->sourceNode->nodeID,
                                             .sourceIndex = connection->sourceIndex,
                                             .destNodeID = connection->destNode->nodeID,
                                             .destIndex = connection->destIndex};

            shaderGraph.sceneData.connections.emplace_back(std::move(connectionData));
        }
        return shaderGraph;
    }

    auto ShaderGraphEditor::getGraphType() const -> ShaderGraphType
    {
        return graphType;
    }

    auto ShaderGraphEditor::compile() -> core::ref_ptr<ShaderGraphResult>
    {
        auto computeResult = sceneGraph->compute<ShaderGraphComputeResult>();
        if (!computeResult)
        {
            return nullptr;
        }

        std::filesystem::path cacheDirPath =
            std::filesystem::path(std::getenv("APPDATA")) / "ionengine-tools" / "Cache";
        cacheDirPath.make_preferred();

        if (!std::filesystem::exists(cacheDirPath))
        {
            if (!std::filesystem::create_directory(cacheDirPath))
            {
                throw core::Exception("An error occurred while creating the directory");
            }
        }

        std::string const fileName = std::filesystem::path(std::tmpnam(nullptr)).filename().string();
        // Save a shader code to file in app's cache folder
        {
            std::ofstream stream(cacheDirPath / fileName);
            if (!stream.is_open())
            {
                throw core::Exception("An error occurred while creating the file");
            }

            stream << computeResult->outputShaderCode;
        }

        std::string const outputName = std::filesystem::path(std::tmpnam(nullptr)).filename().string();

        std::string const inputPath = (cacheDirPath / fileName).generic_string();
        std::string const outputPath = (cacheDirPath / outputName).generic_string();

        std::vector<std::string> const arguments = {"shaderc.exe", "\"" + inputPath + "\"", "--target", "DXIL",
                                                    "--output",    "\"" + outputPath + "\""};

        auto execResult = core::subprocessExecute(arguments);
        if (execResult.has_value())
        {
            auto buffer = std::move(execResult.value());
            std::string const outputMessage(reinterpret_cast<char*>(buffer.data()), buffer.size());

            if (outputMessage.contains(std::format("Out: {}", outputPath)))
            {
                auto output = core::make_ref<ShaderGraphResult>();
                {
                    std::basic_ifstream<uint8_t> stream(outputPath, std::ios::binary);
                    if (!stream.is_open())
                    {
                        return nullptr;
                    }

                    stream.seekg(0, std::ios::end);
                    uint64_t const offset = stream.tellg();
                    stream.seekg(0, std::ios::beg);
                    output->outputShaderData.resize(offset);
                    stream.read(output->outputShaderData.data(), output->outputShaderData.size());
                }
                // std::filesystem::remove(inputPath);
                //std::filesystem::remove(outputPath);
                return output;
            }
            else
            {
                // std::filesystem::remove(inputPath);
                //std::filesystem::remove(outputPath);
                return nullptr;
            }
        }
        else
        {
            return nullptr;
        }
    }
} // namespace ionengine::tools::editor