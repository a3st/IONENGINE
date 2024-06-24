// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "view_model.hpp"
#include "core/exception.hpp"
#include "engine/renderer/shader.hpp"
#include "precompiled.h"
#include <base64pp/base64pp.h>

namespace ionengine::tools::editor
{
    struct AssetOpenFileInfo
    {
        std::string assetName;
        std::string assetType;
        std::string assetPath;

        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.property(assetName, "name");
            archive.property(assetType, "type");
            archive.property(assetPath, "path");
        }
    };

    struct AssetCreateFileInfo
    {
        std::string assetName;
        std::string assetType;
        std::string assetPath;

        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.property(assetName, "name");
            archive.property(assetType, "type");
            archive.property(assetPath, "path");
        }
    };

    struct AssetSaveFileInfo
    {
        std::string assetName;
        std::string assetType;
        std::string assetPath;

        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.property(assetName, "name");
            archive.property(assetType, "type");
            archive.property(assetPath, "path");
        }
    };

    struct AssetDeleteFileInfo
    {
        std::string assetName;
        std::string assetType;
        std::string assetPath;

        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.property(assetName, "name");
            archive.property(assetType, "type");
            archive.property(assetPath, "path");
        }
    };

    struct AssetRenameFileInfo
    {
        std::string assetNewName;
        std::string assetNewPath;
        std::string assetOldName;
        std::string assetOldPath;
        std::string assetType;

        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.property(assetNewName, "newName");
            archive.property(assetNewPath, "newPath");
            archive.property(assetOldName, "oldName");
            archive.property(assetOldPath, "oldPath");
            archive.property(assetType, "type");
        }
    };

    ViewModel::ViewModel(libwebview::App* app)
        : Engine(nullptr), app(app), assetTree("E:\\GitHub\\IONENGINE\\build\\assets")
    {
        app->bind("getAssetTree", [this]() -> std::string { return this->getAssetTree(); });
        app->bind("assetBrowserCreateFile",
                  [this](std::string fileData) -> std::string { return this->assetBrowserCreateFile(fileData); });
        app->bind("assetBrowserDeleteFile",
                  [this](std::string fileData) -> std::string { return this->assetBrowserDeleteFile(fileData); });
        app->bind("assetBrowserRenameFile",
                  [this](std::string fileData) -> std::string { return this->assetBrowserRenameFile(fileData); });
        app->bind("assetBrowserOpenFile",
                  [this](std::string fileData) -> std::string { return this->assetBrowserOpenFile(fileData); });

        app->bind("getShaderGraphComponents", [this]() -> std::string { return this->getShaderGraphComponents(); });
        app->bind("shaderGraphAssetSave", [this](std::string fileData, std::string sceneData) -> std::string {
            return this->shaderGraphAssetSave(fileData, sceneData);
        });
        app->bind("shaderGraphAssetCompile",
                  [this](std::string fileData) -> std::string { return this->shaderGraphAssetCompile(fileData); });

        app->bind("requestViewportTexture", [this]() -> std::string { return this->requestViewportTexture(); });
    }

    auto ViewModel::init() -> void
    {
        viewportTexture = this->createTextureAsset();
        viewportTexture->create(800, 600, TextureUsage::RenderTarget);
    }

    auto ViewModel::update(float const dt) -> void
    {
    }

    auto ViewModel::render() -> void
    {
        std::vector<core::ref_ptr<rhi::Texture>> colors = {viewportTexture->getTexture()};

        renderer.beginDraw(colors, nullptr, math::Color(0.0f, 0.0f, 0.0f, 1.0f), std::nullopt, std::nullopt);

        renderer.render(renderableObjects);

        renderer.endDraw();
    }

    auto ViewModel::requestViewportTexture() -> std::string
    {
        return base64pp::encode(viewportTexture->dump());
    }

    auto ViewModel::getAssetTree() -> std::string
    {
        auto buffer = core::saveToBytes<AssetStructInfo, core::serialize::OutputJSON>(assetTree.fetch()).value();
        return std::string(reinterpret_cast<char*>(buffer.data()), buffer.size());
    }

    auto ViewModel::assetBrowserCreateFile(std::string fileData) -> std::string
    {
        auto result = core::loadFromBytes<AssetCreateFileInfo, core::serialize::InputJSON>(
            std::span<uint8_t>(reinterpret_cast<uint8_t*>(fileData.data()), fileData.size()));
        if (!result.has_value())
        {
            return "{\"error\":0}";
        }

        AssetCreateFileInfo fileInfo = std::move(result.value());

        // Create File
        auto createPath = std::filesystem::path(fileInfo.assetPath).make_preferred();
        {
            uint32_t i = 0;
            std::filesystem::path initialGenPath = createPath;
            while (std::filesystem::exists(initialGenPath))
            {
                initialGenPath = createPath.parent_path() /
                                 std::filesystem::path(createPath.stem().generic_string() + "_Copy_" +
                                                       std::to_string(i) + createPath.extension().generic_string());
                ++i;
            }
            createPath = initialGenPath;

            std::basic_ofstream<uint8_t> stream(createPath, std::ios::binary);
            if (!stream.is_open())
            {
                return "{\"error\":0}";
            }

            if (fileInfo.assetType.compare("asset/shadergraph/unlit") == 0)
            {
                shaderGraphEditor.create(ShaderGraphType::Unlit);

                // Change to general shadergraph type
                // Shadergraph contains type in Output Node's 'userData' options
                fileInfo.assetType = "asset/shadergraph";

                ShaderGraphData graphData = std::move(shaderGraphEditor.dump());
                if (!(core::Serializable<ShaderGraphData>::serialize(graphData, stream) > 0))
                {
                    return "{\"error\":0}";
                }
            }
        }
        fileInfo.assetName = createPath.stem().generic_string();
        fileInfo.assetPath = createPath.generic_string();

        auto buffer = core::saveToBytes<AssetCreateFileInfo, core::serialize::OutputJSON>(fileInfo).value();
        return std::string(reinterpret_cast<char*>(buffer.data()), buffer.size());
    }

    auto ViewModel::assetBrowserDeleteFile(std::string fileData) -> std::string
    {
        auto result = core::loadFromBytes<AssetDeleteFileInfo, core::serialize::InputJSON>(
            std::span<uint8_t>(reinterpret_cast<uint8_t*>(fileData.data()), fileData.size()));
        if (!result.has_value())
        {
            return "{\"error\":0}";
        }

        AssetDeleteFileInfo fileInfo = std::move(result.value());

        if (std::filesystem::remove(std::filesystem::path(fileInfo.assetPath).make_preferred()))
        {
            auto buffer = core::saveToBytes<AssetDeleteFileInfo, core::serialize::OutputJSON>(fileInfo).value();
            return std::string(reinterpret_cast<char*>(buffer.data()), buffer.size());
        }
        else
        {
            return "{\"error\":1}";
        }
    }

    auto ViewModel::assetBrowserRenameFile(std::string fileData) -> std::string
    {
        auto result = core::loadFromBytes<AssetRenameFileInfo, core::serialize::InputJSON>(
            std::span<uint8_t>(reinterpret_cast<uint8_t*>(fileData.data()), fileData.size()));
        if (!result.has_value())
        {
            return "{\"error\":0}";
        }

        AssetRenameFileInfo fileInfo = std::move(result.value());

        auto originalPath = std::filesystem::path(fileInfo.assetOldPath).make_preferred();
        auto newPath = originalPath.parent_path() / std::filesystem::path(std::string(fileInfo.assetNewName) +
                                                                          originalPath.extension().generic_string());

        if (std::filesystem::exists(newPath))
        {
            return "{\"error\":1}";
        }

        std::filesystem::rename(originalPath, newPath);
        fileInfo.assetNewPath = newPath.generic_string();

        auto buffer = core::saveToBytes<AssetRenameFileInfo, core::serialize::OutputJSON>(fileInfo).value();
        return std::string(reinterpret_cast<char*>(buffer.data()), buffer.size());
    }

    auto ViewModel::assetBrowserOpenFile(std::string fileData) -> std::string
    {
        auto result = core::loadFromBytes<AssetOpenFileInfo, core::serialize::InputJSON>(
            std::span<uint8_t>(reinterpret_cast<uint8_t*>(fileData.data()), fileData.size()));
        if (!result.has_value())
        {
            return "{\"error\":0}";
        }

        AssetOpenFileInfo fileInfo = std::move(result.value());

        if (fileInfo.assetType.compare("asset/shadergraph") == 0)
        {
            if (!shaderGraphEditor.loadFromFile(std::filesystem::path(fileInfo.assetPath).make_preferred()))
            {
                return "{\"error\":1}";
            }

            auto buffer =
                core::saveToBytes<ShaderGraphData, core::serialize::OutputJSON>(shaderGraphEditor.dump()).value();
            return std::string(reinterpret_cast<char*>(buffer.data()), buffer.size());
        }
        else
        {
            return "{\"error\":2}";
        }
    }

    auto ViewModel::getShaderGraphComponents() -> std::string
    {
        auto components = shaderGraphEditor.getComponentRegistry().getComponents() |
                          std::views::filter([](auto const& element) { return element.second->isContextRegister(); });

        std::stringstream stream;
        stream << "{\"items\":[";

        bool isFirst = true;
        for (auto const& [componentID, nodeComponent] : components)
        {
            if (!isFirst)
            {
                stream << ",";
            }

            auto result = nodeComponent->getGroupName();
            std::string groupName;
            if (result.has_value())
            {
                groupName = std::move(result.value());
            }
            else
            {
                groupName = "Ungroup";
            }

            stream << "{\"name\":\"" << nodeComponent->getName() << "\",\"group\":\"" << groupName
                   << "\",\"fixed\":" << nodeComponent->isFixed() << ",\"inputs\":[";

            isFirst = true;
            for (auto const& socket : nodeComponent->setInputs())
            {
                if (!isFirst)
                {
                    stream << ",";
                }

                stream << "{\"name\":\"" << socket.socketName << "\",\"type\":\"" << socket.socketType << "\"}";
                isFirst = false;
            }

            stream << "],\"outputs\":[";

            isFirst = true;
            for (auto const& socket : nodeComponent->setOutputs())
            {
                if (!isFirst)
                {
                    stream << ",";
                }

                stream << "{\"name\":\"" << socket.socketName << "\",\"type\":\"" << socket.socketType << "\"}";
                isFirst = false;
            }

            stream << "],\"userData\":{\"componentID\":" << nodeComponent->componentID << ",\"options\":{";

            isFirst = true;
            for (auto const& [key, value] : nodeComponent->setOptions())
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

        stream << "]}";
        return stream.str();
    }

    auto ViewModel::shaderGraphAssetSave(std::string fileData, std::string sceneData) -> std::string
    {
        auto result = core::loadFromBytes<AssetSaveFileInfo, core::serialize::InputJSON>(
            std::span<uint8_t>(reinterpret_cast<uint8_t*>(fileData.data()), fileData.size()));
        if (!result.has_value())
        {
            return "{\"error\":0}";
        }

        AssetSaveFileInfo fileInfo = std::move(result.value());

        simdjson::ondemand::parser parser;
        auto document = parser.iterate(sceneData, sceneData.size() + simdjson::SIMDJSON_PADDING);

        ShaderGraphData graphData = {};

        uint64_t graphType;
        auto error = document["graphType"].get_uint64().get(graphType);
        if (error != simdjson::SUCCESS)
        {
            return "{\"error\":0}";
        }
        graphData.graphType = static_cast<ShaderGraphType>(graphType);

        simdjson::ondemand::object sceneObject;
        error = document["sceneData"].get_object().get(sceneObject);

        simdjson::ondemand::array nodes;
        error = sceneObject["nodes"].get_array().get(nodes);
        if (error != simdjson::SUCCESS)
        {
            return "{\"error\":0}";
        }

        for (auto node : nodes)
        {
            NodeData nodeData = {};

            uint64_t nodeID;
            error = node["id"].get_uint64().get(nodeID);
            if (error != simdjson::SUCCESS)
            {
                return "{\"error\":1}";
            }

            nodeData.nodeID = nodeID;

            simdjson::ondemand::array positions;
            error = node["position"].get_array().get(positions);
            if (error != simdjson::SUCCESS)
            {
                return "{\"error\":2}";
            }

            int32_t i = 0;
            std::array<int32_t, 2> arrayOfPositions;
            for (auto position : positions)
            {
                int64_t value;
                error = position.get_int64().get(value);
                if (error != simdjson::SUCCESS)
                {
                    return "{\"error\":3}";
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
                return "{\"error\":4}";
            }

            uint64_t componentID;
            error = userData["componentID"].get_uint64().get(componentID);
            if (error != simdjson::SUCCESS)
            {
                return "{\"error\":5}";
            }

            nodeData.componentID = componentID;

            simdjson::ondemand::object options;
            userData["options"].get_object().get(options);
            if (error != simdjson::SUCCESS)
            {
                return "{\"error\":6}";
            }

            for (auto element : options)
            {
                std::string_view key;
                error = element.unescaped_key().get(key);
                if (error != simdjson::SUCCESS)
                {
                    return "{\"error\":7}";
                }

                std::string_view value;
                error = element.value().get_string().get(value);
                if (error != simdjson::SUCCESS)
                {
                    return "{\"error\":8}";
                }

                nodeData.options[std::string(key)] = std::string(value);
            }

            simdjson::ondemand::array inputs;
            error = node["inputs"].get_array().get(inputs);
            if (error != simdjson::SUCCESS)
            {
                return "{\"error\":9}";
            }

            for (auto input : inputs)
            {
                std::string_view socketName;
                error = input["name"].get_string().get(socketName);
                if (error != simdjson::SUCCESS)
                {
                    return "{\"error\":9}";
                }

                std::string_view socketType;
                error = input["type"].get_string().get(socketType);
                if (error != simdjson::SUCCESS)
                {
                    return "{\"error\":9}";
                }

                NodeSocketData socketData = {.socketName = std::string(socketName),
                                             .socketType = std::string(socketType)};
                nodeData.inputs.emplace_back(std::move(socketData));
            }

            simdjson::ondemand::array outputs;
            error = node["outputs"].get_array().get(outputs);
            if (error != simdjson::SUCCESS)
            {
                return "{\"error\":10}";
            }

            for (auto output : outputs)
            {
                std::string_view socketName;
                error = output["name"].get_string().get(socketName);
                if (error != simdjson::SUCCESS)
                {
                    return "{\"error\":10}";
                }

                std::string_view socketType;
                error = output["type"].get_string().get(socketType);
                if (error != simdjson::SUCCESS)
                {
                    return "{\"error\":10}";
                }

                NodeSocketData socketData = {.socketName = std::string(socketName),
                                             .socketType = std::string(socketType)};
                nodeData.outputs.emplace_back(std::move(socketData));
            }

            graphData.nodes.emplace_back(std::move(nodeData));
        }

        simdjson::ondemand::array connections;
        error = sceneObject["connections"].get_array().get(connections);
        if (error != simdjson::SUCCESS)
        {
            return "{\"error\":9}";
        }

        for (auto connection : connections)
        {
            ConnectionData connectionData = {};

            uint64_t connectionID;
            auto error = connection["id"].get_uint64().get(connectionID);
            if (error != simdjson::SUCCESS)
            {
                return "{\"error\":10}";
            }

            connectionData.connectionID = connectionID;

            uint64_t sourceNodeID;
            error = connection["source"].get_uint64().get(sourceNodeID);
            if (error != simdjson::SUCCESS)
            {
                return "{\"error\":11}";
            }

            connectionData.source = sourceNodeID;

            uint64_t sourceIndex;
            error = connection["out"].get_uint64().get(sourceIndex);
            if (error != simdjson::SUCCESS)
            {
                return "{\"error\":13}";
            }

            connectionData.out = sourceIndex;

            uint64_t destNodeID;
            error = connection["dest"].get_uint64().get(destNodeID);
            if (error != simdjson::SUCCESS)
            {
                return "{\"error\":12}";
            }

            connectionData.dest = destNodeID;

            uint64_t destIndex;
            error = connection["in"].get_uint64().get(destIndex);
            if (error != simdjson::SUCCESS)
            {
                return "{\"error\":14}";
            }

            connectionData.in = destIndex;

            graphData.connections.emplace_back(std::move(connectionData));
        }

        if (core::saveToFile<ShaderGraphData>(graphData, std::filesystem::path(fileInfo.assetPath).make_preferred()))
        {
            auto buffer = core::saveToBytes<AssetSaveFileInfo, core::serialize::OutputJSON>(fileInfo).value();
            return std::string(reinterpret_cast<char*>(buffer.data()), buffer.size());
        }
        else
        {
            return "{\"error\":16}";
        }
    }

    auto ViewModel::shaderGraphAssetCompile(std::string fileData) -> std::string
    {
        auto result = core::loadFromBytes<AssetOpenFileInfo, core::serialize::InputJSON>(
            std::span<uint8_t>(reinterpret_cast<uint8_t*>(fileData.data()), fileData.size()));
        if (!result.has_value())
        {
            return "{\"error\":0}";
        }

        AssetOpenFileInfo fileInfo = std::move(result.value());

        if (!shaderGraphEditor.loadFromFile(fileInfo.assetPath))
        {
            return "{\"error\":1}";
        }

        auto compileResult = shaderGraphEditor.compile();
        if (!compileResult)
        {
            return "{\"error\":2}";
        }

        outputShader = createShaderAsset();
        if (!outputShader->loadFromBytes(compileResult->outputShaderData))
        {
            return "{\"error\":3}";
        }

        RenderableData renderableData = {.renderableType = RenderableType::Quad,
                                         .shader = outputShader->getShader(),
                                         .rasterizerStage = outputShader->getRasterizerStage(),
                                         .blendColor = rhi::BlendColorInfo::Opaque(),
                                         .depthStencil = std::nullopt};

        renderableObjects.clear();
        renderableObjects.emplace_back(std::move(renderableData));
        return "{\"ok\":0}";
    }
} // namespace ionengine::tools::editor