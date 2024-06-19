// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "view_model.hpp"
#include "core/exception.hpp"
#include "engine/renderer/shader.hpp"
#include "precompiled.h"
#include <base64pp/base64pp.h>

namespace ionengine::tools::editor
{
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

        renderer.endDraw();
    }

    auto ViewModel::requestViewportTexture() -> std::string
    {
        return base64pp::encode(viewportTexture->dump());
    }

    auto ViewModel::getAssetTree() -> std::string
    {
        std::stringstream stream;
        auto internalLoop = [](this auto const& internalLoop, AssetStructInfo const* curStruct,
                               std::stringstream& stream) -> void {
            std::string assetType;
            bool isFolder = false;
            switch (curStruct->type)
            {
                case AssetType::Folder: {
                    assetType = "folder";
                    isFolder = true;
                    break;
                }
                case AssetType::Asset: {
                    std::basic_ifstream<uint8_t> stream(curStruct->path, std::ios::binary);
                    auto result = getAssetHeader(stream);
                    if (result.has_value())
                    {
                        asset::Header header = std::move(result.value());
                        if (std::memcmp(header.fileType.data(), ShaderGraphFileType.data(),
                                        ShaderGraphFileType.size()) == 0)
                        {
                            assetType = "asset/shadergraph";
                        }
                        else
                        {
                            assetType = "asset/unknown";
                        }
                    }
                    else
                    {
                        assetType = "file/unknown";
                    }
                    break;
                }
                default: {
                    assetType = "file/unknown";
                    break;
                }
            }

            stream << "{\"name\":\"" << curStruct->name << "\",\"type\":\"" << assetType << "\",\"path\":\""
                   << curStruct->path.generic_string() << "\"";

            if (isFolder)
            {
                stream << ",\"childrens\":[";
                for (auto const i : std::views::iota(0u, curStruct->childrens.size()))
                {
                    internalLoop(curStruct->childrens[i].get(), stream);
                }
                stream << "]";
            }

            stream << "},";
        };

        internalLoop(&assetTree.fetch(), stream);

        // Idk how realize algorithm through isFirst, !isFirst
        std::regex const jsonCommaRemove("\\,\\]");

        std::string jsonData = stream.str();
        jsonData = jsonData.substr(0, jsonData.size() - 1);

        return std::regex_replace(jsonData, jsonCommaRemove, "]");
    }

    auto ViewModel::assetBrowserCreateFile(std::string fileData) -> std::string
    {
        simdjson::ondemand::parser parser;
        auto document = parser.iterate(fileData, fileData.size() + simdjson::SIMDJSON_PADDING);

        std::string_view filePath;
        auto error = document["path"].get_string().get(filePath);
        if (error != simdjson::SUCCESS)
        {
            return "{\"error\":0}";
        }

        std::string_view fileName;
        error = document["name"].get_string().get(fileName);
        if (error != simdjson::SUCCESS)
        {
            return "{\"error\":0}";
        }

        std::string_view fileType;
        error = document["type"].get_string().get(fileType);
        if (error != simdjson::SUCCESS)
        {
            return "{\"error\":0}";
        }

        // Create File
        std::string assetType(fileType);
        auto createPath = std::filesystem::path(filePath).make_preferred();
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

            if (fileType.compare("asset/shadergraph/unlit") == 0)
            {
                shaderGraphEditor.create(ShaderGraphType::Unlit);

                // Change to general shadergraph type
                // Shadergraph contains type in Output Node's 'userData' options
                assetType = "asset/shadergraph";

                ShaderGraphData graphData = std::move(shaderGraphEditor.dump());
                if (!(core::Serializable<ShaderGraphData>::serialize(graphData, stream) > 0))
                {
                    return "{\"error\":0}";
                }
            }
        }

        std::stringstream stream;
        stream << "{\"name\":\"" << createPath.stem().generic_string() << "\",\"type\":\"" << assetType
               << "\",\"path\":\"" << createPath.generic_string() << "\"}";
        return stream.str();
    }

    auto ViewModel::assetBrowserDeleteFile(std::string fileData) -> std::string
    {
        simdjson::ondemand::parser parser;
        auto document = parser.iterate(fileData, fileData.size() + simdjson::SIMDJSON_PADDING);

        std::string_view filePath;
        auto error = document["path"].get_string().get(filePath);
        if (error != simdjson::SUCCESS)
        {
            return "{\"error\":0}";
        }

        std::string_view fileName;
        error = document["name"].get_string().get(fileName);
        if (error != simdjson::SUCCESS)
        {
            return "{\"error\":0}";
        }

        std::string_view fileType;
        error = document["type"].get_string().get(fileType);
        if (error != simdjson::SUCCESS)
        {
            return "{\"error\":0}";
        }

        if (std::filesystem::remove(std::filesystem::path(filePath).make_preferred()))
        {
            std::stringstream stream;
            stream << "{\"name\":\"" << fileName << "\",\"type\":\"" << fileType << "\",\"path\":\"" << filePath
                   << "\"}";
            return stream.str();
        }
        else
        {
            return "{\"error\":0}";
        }
    }

    auto ViewModel::assetBrowserRenameFile(std::string fileData) -> std::string
    {
        simdjson::ondemand::parser parser;
        auto document = parser.iterate(fileData, fileData.size() + simdjson::SIMDJSON_PADDING);

        std::string_view oldFileName;
        auto error = document["oldName"].get_string().get(oldFileName);
        if (error != simdjson::SUCCESS)
        {
            return "{\"error\":0}";
        }

        std::string_view newFileName;
        error = document["newName"].get_string().get(newFileName);
        if (error != simdjson::SUCCESS)
        {
            return "{\"error\":0}";
        }

        std::string_view fileType;
        error = document["type"].get_string().get(fileType);
        if (error != simdjson::SUCCESS)
        {
            return "{\"error\":0}";
        }

        std::string_view oldFilePath;
        error = document["oldPath"].get_string().get(oldFilePath);
        if (error != simdjson::SUCCESS)
        {
            return "{\"error\":0}";
        }

        auto originalPath = std::filesystem::path(oldFilePath).make_preferred();
        auto newPath = originalPath.parent_path() /
                       std::filesystem::path(std::string(newFileName) + originalPath.extension().generic_string());

        std::cout << originalPath << std::endl;
        std::cout << newPath << std::endl;

        if (std::filesystem::exists(newPath))
        {
            return "{\"error\":23}";
        }

        std::filesystem::rename(originalPath, newPath);

        std::stringstream stream;
        stream << "{\"newName\":\"" << newFileName << "\",\"type\":\"" << fileType << "\",\"oldPath\":\""
               << originalPath.generic_string() << "\",\"oldName\":\"" << oldFileName << "\",\"newPath\":\""
               << newPath.generic_string() << "\"}";

        return stream.str();
    }

    auto ViewModel::assetBrowserOpenFile(std::string fileData) -> std::string
    {
        simdjson::ondemand::parser parser;
        auto document = parser.iterate(fileData, fileData.size() + simdjson::SIMDJSON_PADDING);

        std::string_view filePath;
        auto error = document["path"].get_string().get(filePath);
        if (error != simdjson::SUCCESS)
        {
            return "{\"error\":0}";
        }

        std::string_view fileName;
        error = document["name"].get_string().get(fileName);
        if (error != simdjson::SUCCESS)
        {
            return "{\"error\":1}";
        }

        std::string_view fileType;
        error = document["type"].get_string().get(fileType);
        if (error != simdjson::SUCCESS)
        {
            return "{\"error\":2}";
        }

        if (fileType.compare("asset/shadergraph") == 0)
        {
            if (!shaderGraphEditor.loadFromFile(std::filesystem::path(filePath).make_preferred()))
            {
                return "{\"error\":3}";
            }
            return shaderGraphDataToJSON(shaderGraphEditor.dump());
        }
        else
        {
            return "{\"error\":4}";
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

    auto ViewModel::shaderGraphDataToJSON(ShaderGraphData const& graphData) -> std::string
    {
        std::stringstream stream;

        std::string graphType;
        switch (graphData.graphType)
        {
            case ShaderGraphType::Lit: {
                graphType = "lit";
                break;
            }
            case ShaderGraphType::Unlit: {
                graphType = "unlit";
                break;
            }
        }

        stream << "{\"graphType\":\"" << graphType << "\",\"sceneData\":{\"nodes\":[";

        bool isFirst = true;
        for (auto const& node : graphData.nodes)
        {
            if (!isFirst)
            {
                stream << ",";
            }

            auto const nodeComponent = shaderGraphEditor.getComponentRegistry().getComponents().at(node.componentID);

            stream << "{\"id\":" << node.nodeID << ",\"position\":[" << node.posX << "," << node.posY << "],\"name\":\""
                   << nodeComponent->getName() << "\",\"fixed\":" << std::boolalpha << nodeComponent->isFixed()
                   << ",\"inputs\":[";

            isFirst = true;
            for (auto const& input : node.inputs)
            {
                if (!isFirst)
                {
                    stream << ",";
                }

                stream << "{\"type\":\"" << input.socketType << "\",\"name\":\"" << input.socketName << "\"}";
                isFirst = false;
            }

            stream << "],\"outputs\":[";

            isFirst = true;
            for (auto const& output : node.outputs)
            {
                if (!isFirst)
                {
                    stream << ",";
                }

                stream << "{\"type\":\"" << output.socketType << "\",\"name\":\"" << output.socketName << "\"}";
                isFirst = false;
            }

            stream << "],\"userData\":{\"componentID\":" << nodeComponent->componentID << ",\"options\":{";

            isFirst = true;
            for (auto const& [key, value] : node.options)
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
        for (auto const& connection : graphData.connections)
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

        stream << "]}}";
        return stream.str();
    }

    auto ViewModel::shaderGraphAssetSave(std::string fileData, std::string sceneData) -> std::string
    {
        simdjson::ondemand::parser parser;
        auto document = parser.iterate(sceneData, sceneData.size() + simdjson::SIMDJSON_PADDING);

        ShaderGraphData graphData = {};

        simdjson::ondemand::array nodes;
        auto error = document["nodes"].get_array().get(nodes);
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

            graphData.nodes.emplace_back(std::move(nodeData));
        }

        simdjson::ondemand::array connections;
        error = document["connections"].get_array().get(connections);
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

        document = parser.iterate(fileData, fileData.size() + simdjson::SIMDJSON_PADDING);

        std::string_view filePath;
        error = document["path"].get_string().get(filePath);
        if (error != simdjson::SUCCESS)
        {
            return "{\"error\":15}";
        }

        if (core::saveToFile<ShaderGraphData>(graphData, std::filesystem::path(filePath).make_preferred()))
        {
            return fileData;
        }
        else
        {
            return "{\"error\":16}";
        }
    }

    auto ViewModel::shaderGraphAssetCompile(std::string fileData) -> std::string
    {
        simdjson::ondemand::parser parser;
        auto document = parser.iterate(fileData, fileData.size() + simdjson::SIMDJSON_PADDING);

        std::string_view filePath;
        auto error = document["path"].get_string().get(filePath);
        if (error != simdjson::SUCCESS)
        {
            return "{\"error\":0}";
        }

        if (!shaderGraphEditor.loadFromFile(filePath))
        {
            return "{\"error\":1}";
        }

        auto result = shaderGraphEditor.compile();
        if (!result)
        {
            return "{\"error\":2}";
        }

        outputShader = createShaderAsset();
        if (!outputShader->loadFromBytes(result->outputShaderData))
        {
            return "{\"error\":3}";
        }
        return "{\"ok\":0}";
    }
} // namespace ionengine::tools::editor