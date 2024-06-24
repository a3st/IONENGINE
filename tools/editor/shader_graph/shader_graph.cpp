// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "shader_graph.hpp"
#include "core/exception.hpp"
#include "core/subprocess.hpp"
#include "engine/asset.hpp"
#include "precompiled.h"
#include <simdjson.h>

#include "shader_graph/components/convert.hpp"
#include "shader_graph/components/input.hpp"
#include "shader_graph/components/math.hpp"
#include "shader_graph/components/output.hpp"
#include "shader_graph/components/texture.hpp"

namespace ionengine::tools::editor
{
    namespace internal
    {
        auto deserialize(std::basic_istream<uint8_t>& stream) -> std::optional<ShaderGraphData>
        {
            asset::Header header;
            stream.read((uint8_t*)&header, sizeof(asset::Header));

            if (std::memcmp(header.magic.data(), asset::Magic.data(), asset::Magic.size()) != 0)
            {
                return std::nullopt;
            }

            if (std::memcmp(header.fileType.data(), ShaderGraphFileType.data(), ShaderGraphFileType.size()) != 0)
            {
                return std::nullopt;
            }

            asset::ChunkHeader chunkHeader;
            stream.read((uint8_t*)&chunkHeader, sizeof(asset::ChunkHeader));

            std::vector<uint8_t> chunkJsonData(chunkHeader.chunkLength);
            stream.read(chunkJsonData.data(), chunkJsonData.size());

            ShaderGraphData shaderGraph = {};

            if (!readJsonChunkData(shaderGraph, chunkJsonData))
            {
                return std::nullopt;
            }
            return shaderGraph;
        }

        auto serialize(ShaderGraphData const& object, std::basic_ostream<uint8_t>& stream) -> size_t
        {
            std::string const jsonData = generateJsonChunkData(object);

            asset::Header header = {.magic = asset::Magic, .length = 0, .fileType = ShaderGraphFileType};
            stream.write(reinterpret_cast<uint8_t*>(&header), sizeof(asset::Header));
            asset::ChunkHeader jsonChunkHeader = {.chunkType = static_cast<uint32_t>(asset::ChunkType::JSON),
                                                  .chunkLength = static_cast<uint32_t>(jsonData.size())};
            stream.write(reinterpret_cast<uint8_t const*>(&jsonChunkHeader), sizeof(asset::ChunkHeader));
            stream.write(reinterpret_cast<uint8_t const*>(jsonData.data()), jsonData.size());
            uint64_t const offset = stream.tellp();
            header.length = offset;
            stream.seekp(0, std::ios::beg);
            stream.write(reinterpret_cast<uint8_t*>(&header), sizeof(asset::Header));
            return offset;
        }
    } // namespace internal

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
        sceneGraph = core::make_ref<Scene>(componentRegistry);

        auto result = core::loadFromFile<ShaderGraphData>(filePath);
        if (!result.has_value())
        {
            return false;
        }

        ShaderGraphData graphData = std::move(result.value());
        graphType = graphData.graphType;

        std::unordered_map<uint64_t, core::ref_ptr<Node>> componentsOfNodes;

        for (auto const& node : graphData.nodes)
        {
            auto createdNode = sceneGraph->createNodeByID(node.componentID, node.nodeID);
            componentsOfNodes.emplace(node.nodeID, createdNode);

            createdNode->posX = node.posX;
            createdNode->posY = node.posY;

            for (auto const& [key, value] : node.options)
            {
                createdNode->options[key] = value;
            }

            if (!node.inputs.empty())
            {
                createdNode->inputs.clear();

                for (auto const& input : node.inputs)
                {
                    Node::SocketInfo socketInfo = {.socketName = input.socketName, .socketType = input.socketType};
                    createdNode->inputs.emplace_back(std::move(socketInfo));
                }
            }

            if (!node.outputs.empty())
            {
                createdNode->outputs.clear();

                for (auto const& output : node.outputs)
                {
                    Node::SocketInfo socketInfo = {.socketName = output.socketName, .socketType = output.socketType};
                    createdNode->outputs.emplace_back(std::move(socketInfo));
                }
            }
        }

        for (auto const& connection : graphData.connections)
        {
            sceneGraph->createConnection(connection.connectionID, componentsOfNodes[connection.source], connection.out,
                                         componentsOfNodes[connection.dest], connection.in);
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
            NodeData nodeData = {};
            nodeData.nodePosition[0] = node->posX;
            nodeData.nodePosition[1] = node->posY;
            nodeData.nodeID = node->nodeID;
            nodeData.nodeUserData.nodeComponentID = node->componentID;
            nodeData.nodeUserData.nodeOptions = node->options;

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

            for (auto const& [key, value] : node->options)
            {
                nodeData.nodeUserData.nodeOptions[key] = value;
            }

            shaderGraph.sceneData.nodes.emplace_back(std::move(nodeData));
        }

        for (auto const& connection : sceneGraph->getConnections())
        {
            ConnectionData connectionData = {};
            connectionData.connectionID = connection->connectionID;
            connectionData.sourceNodeID = connection->sourceNode->nodeID;
            connectionData.sourceIndex = connection->sourceIndex;
            connectionData.destNodeID = connection->destNode->nodeID;
            connectionData.destIndex = connection->destIndex;

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
                std::filesystem::remove(outputPath);
                return output;
            }
            else
            {
                // std::filesystem::remove(inputPath);
                std::filesystem::remove(outputPath);
                return nullptr;
            }
        }
        else
        {
            return nullptr;
        }
    }
} // namespace ionengine::tools::editor