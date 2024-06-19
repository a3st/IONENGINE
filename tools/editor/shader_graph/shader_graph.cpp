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
    auto readJsonChunkData(ShaderGraphData& object, std::vector<uint8_t>& data) -> bool
    {
        simdjson::ondemand::parser parser;
        auto document = parser.iterate(data.data(), data.size(), data.size() + simdjson::SIMDJSON_PADDING);

        simdjson::ondemand::array nodes;
        auto error = document["nodes"].get_array().get(nodes);
        if (error != simdjson::SUCCESS)
        {
            return false;
        }

        for (auto node : nodes)
        {
            NodeData nodeData = {};

            uint64_t nodeID;
            error = node["id"].get_uint64().get(nodeID);
            if (error != simdjson::SUCCESS)
            {
                return false;
            }

            nodeData.nodeID = nodeID;

            simdjson::ondemand::array positions;
            error = node["position"].get_array().get(positions);
            if (error != simdjson::SUCCESS)
            {
                return false;
            }

            int32_t i = 0;
            std::array<int32_t, 2> arrayOfPositions;
            for (auto position : positions)
            {
                int64_t value;
                error = position.get_int64().get(value);
                if (error != simdjson::SUCCESS)
                {
                    return false;
                }
                arrayOfPositions[i] = value;
                ++i;
            }

            nodeData.posX = arrayOfPositions[0];
            nodeData.posY = arrayOfPositions[1];

            simdjson::ondemand::object userData;
            error = node["userData"].get_object().get(userData);
            if (error != simdjson::SUCCESS)
            {
                return false;
            }

            uint64_t componentID;
            error = userData["componentID"].get_uint64().get(componentID);
            if (error != simdjson::SUCCESS)
            {
                return false;
            }

            nodeData.componentID = componentID;

            simdjson::ondemand::object options;
            userData["options"].get_object().get(options);
            if (error != simdjson::SUCCESS)
            {
                return false;
            }

            for (auto element : options)
            {
                std::string_view key;
                error = element.unescaped_key().get(key);
                if (error != simdjson::SUCCESS)
                {
                    return false;
                }

                std::string_view value;
                error = element.value().get_string().get(value);
                if (error != simdjson::SUCCESS)
                {
                    return false;
                }

                nodeData.options[std::string(key)] = std::string(value);
            }

            object.nodes.emplace_back(std::move(nodeData));
        }

        simdjson::ondemand::array connections;
        error = document["connections"].get_array().get(connections);
        if (error != simdjson::SUCCESS)
        {
            return false;
        }

        for (auto connection : connections)
        {
            ConnectionData connectionData = {};

            uint64_t connectionID;
            auto error = connection["id"].get_uint64().get(connectionID);
            if (error != simdjson::SUCCESS)
            {
                return false;
            }

            connectionData.connectionID = connectionID;

            uint64_t sourceNodeID;
            error = connection["source"].get_uint64().get(sourceNodeID);
            if (error != simdjson::SUCCESS)
            {
                return false;
            }

            connectionData.source = sourceNodeID;

            uint64_t sourceIndex;
            error = connection["out"].get_uint64().get(sourceIndex);
            if (error != simdjson::SUCCESS)
            {
                return false;
            }

            connectionData.out = sourceIndex;

            uint64_t destNodeID;
            error = connection["dest"].get_uint64().get(destNodeID);
            if (error != simdjson::SUCCESS)
            {
                return false;
            }

            connectionData.dest = destNodeID;

            uint64_t destIndex;
            error = connection["in"].get_uint64().get(destIndex);
            if (error != simdjson::SUCCESS)
            {
                return false;
            }

            connectionData.in = destIndex;

            object.connections.emplace_back(std::move(connectionData));
        }
        return true;
    }

    auto generateJsonChunkData(ShaderGraphData const& object) -> std::string
    {
        std::stringstream stream;
        stream << "{\"nodes\":[";

        bool isFirst = true;
        for (auto const& node : object.nodes)
        {
            if (!isFirst)
            {
                stream << ",";
            }

            stream << "{\"id\":" << node.nodeID << ",\"position\":[" << node.posX << "," << node.posY
                   << "],\"userData\":{\"componentID\":" << node.componentID << ",\"options\":{";

            isFirst = true;
            for (auto const [key, value] : node.options)
            {
                if (!isFirst)
                {
                    stream << ",";
                }

                stream << "\"" << key << "\":\"" << value << "\"";
                isFirst = false;
            }
            stream << "}}}";
            isFirst = false;
        }
        stream << "],\"connections\":[";

        isFirst = true;
        for (auto const& connection : object.connections)
        {
            if (!isFirst)
            {
                stream << ",";
            }

            stream << "{\"id\":" << connection.connectionID << ",\"source\":" << connection.source
                   << ",\"out\":" << connection.out << ",\"dest\":" << connection.dest << ",\"in\":" << connection.in
                   << "}";

            isFirst = false;
        }
        stream << "]}";

        return stream.str();
    }

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

            if (createdNode->inputs.empty())
            {
                for (auto const& input : node.inputs)
                {
                    Node::SocketInfo socketInfo = {.socketName = input.socketName, .socketType = input.socketType};
                    createdNode->inputs.emplace_back(std::move(socketInfo));
                }
            }
            if (createdNode->outputs.empty())
            {
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
            nodeData.posX = node->posX;
            nodeData.posY = node->posY;
            nodeData.nodeID = node->nodeID;
            nodeData.options = node->options;
            nodeData.componentID = node->componentID;

            for (auto const& input : node->inputs)
            {
                NodeSocketData socketData = {.socketName = input.socketName, .socketType = input.socketType};
                nodeData.inputs.emplace_back(std::move(socketData));
            }

            for (auto const& output : node->outputs)
            {
                NodeSocketData socketData = {.socketName = output.socketName, .socketType = output.socketType};
                nodeData.outputs.emplace_back(std::move(socketData));
            }

            shaderGraph.nodes.emplace_back(std::move(nodeData));
        }

        for (auto const& connection : sceneGraph->getConnections())
        {
            ConnectionData connectionData = {};
            connectionData.connectionID = connection->connectionID;
            connectionData.source = connection->sourceNode->nodeID;
            connectionData.out = connection->sourceIndex;
            connectionData.dest = connection->destNode->nodeID;
            connectionData.in = connection->destIndex;

            shaderGraph.connections.emplace_back(std::move(connectionData));
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
                std::filesystem::remove(inputPath);
                std::filesystem::remove(outputPath);
                return output;
            }
            else
            {
                std::filesystem::remove(inputPath);
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