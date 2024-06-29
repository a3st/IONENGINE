// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "editor.hpp"
#include "core/subprocess.hpp"
#include "precompiled.h"

namespace ionengine::tools::editor
{
    ShaderGraphEditor::ShaderGraphEditor(ComponentRegistry& componentRegistry) : componentRegistry(&componentRegistry)
    {
    }

    auto ShaderGraphEditor::loadAsset(core::ref_ptr<ShaderGraphAsset> asset) -> void
    {
        sceneGraph = core::make_ref<Scene>(*componentRegistry);

        std::unordered_map<uint64_t, core::ref_ptr<Node>> componentsOfNodes;

        for (auto const& node : asset->getGraphData().sceneData.nodes)
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

        for (auto const& connection : asset->getGraphData().sceneData.connections)
        {
            sceneGraph->createConnection(connection.connectionID, componentsOfNodes[connection.sourceNodeID],
                                         connection.sourceIndex, componentsOfNodes[connection.destNodeID],
                                         connection.destIndex);
        }

        loadedAsset = asset;
    }

    auto ShaderGraphEditor::getAsset() -> core::ref_ptr<ShaderGraphAsset>
    {
        return loadedAsset;
    }

    auto ShaderGraphEditor::getScene() -> core::ref_ptr<Scene>
    {
        return sceneGraph;
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
                // std::filesystem::remove(outputPath);
                return output;
            }
            else
            {
                // std::filesystem::remove(inputPath);
                // std::filesystem::remove(outputPath);
                return nullptr;
            }
        }
        else
        {
            return nullptr;
        }
    }
} // namespace ionengine::tools::editor