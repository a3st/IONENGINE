// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "view_model.hpp"
#include "core/exception.hpp"
#include "core/subprocess.hpp"
#include "engine/renderer/shader.hpp"
#include "precompiled.h"
#include <base64pp/base64pp.h>

namespace ionengine::tools::editor
{
    ViewModel::ViewModel(libwebview::App* app) : Engine(nullptr), app(app), assetTree("../assets")
    {
        app->bind("getAssetTree", [this]() -> std::string { return this->getAssetTree(); });
        app->bind("assetBrowserCreateFile",
                  [this](std::string fileData) -> std::string { return this->assetBrowserCreateFile(fileData); });

        app->bind("addContextItems", [this]() -> std::string { return this->addContextItems(); });
        app->bind("addResourceTypes", [this]() -> std::string { return this->addResourceTypes(); });
        // app->bind("addShaderDomains", [this]() -> std::string { return this->addShaderDomains(); });
        app->bind("requestPreviewImage", [this]() -> std::string { return this->requestPreviewImage(); });
        app->bind("compileShader",
                  [this](std::string sceneData) -> std::string { return this->compileShader(sceneData); });
    }

    auto ViewModel::init() -> void
    {
        previewImage = this->createTextureAsset();
        previewImage->create(800, 600, TextureUsage::RenderTarget);
    }

    auto ViewModel::update(float const dt) -> void
    {
    }

    auto ViewModel::render() -> void
    {
        std::vector<core::ref_ptr<rhi::Texture>> colors = {previewImage->getTexture()};

        renderer.beginDraw(colors, nullptr, math::Color(0.2f, 0.3f, 0.4f, 1.0f), std::nullopt, std::nullopt);

        if (previewShader)
        {
            renderer.setShader("genShader");
            renderer.drawQuad({});
        }

        renderer.endDraw();
    }

    auto ViewModel::requestPreviewImage() -> std::string
    {
        return base64pp::encode(previewImage->dump());
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

        std::basic_ofstream<uint8_t> stream;
        if (assetTree.createFile(
                std::filesystem::path(filePath) / std::filesystem::path(std::string(fileName) + ".asset"), stream))
        {
            if (fileType.compare("asset/shadergraph") == 0)
            {
                shaderGraphEditor.create();
                ShaderGraphData graphData = std::move(shaderGraphEditor.dump());
                if (core::Serializable<ShaderGraphData>::serialize(graphData, stream) > 0)
                {

                }
                else
                {
                    return "{\"error\":0}";
                }
            }

            return "{}";
        }
        else
        {
            return "{\"error\":0}";
        }
    }

    auto ViewModel::addContextItems() -> std::string
    {
        auto components = componentRegistry.getComponents() |
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

    auto ViewModel::addResourceTypes() -> std::string
    {
        std::stringstream stream;
        stream << "{\"types\":[";

        bool isFirst = true;
        for (auto const& resourceType : {"Texture2D", "float4", "float3", "float2", "float", "bool"})
        {
            if (!isFirst)
            {
                stream << ",";
            }

            stream << "\"" << resourceType << "\"";
            isFirst = false;
        }

        stream << "]}";
        return stream.str();
    }

    /*auto ViewModel::addShaderDomains() -> std::string
    {
        auto components = componentRegistry.getComponents() | std::views::filter([](auto const& element) {
                              return element.second->isFixed() && element.second->getName().compare("Output Node") == 0;
                          });

        std::unordered_map<uint32_t, std::string> shaderDomainNames = {
            {componentRegistry.getComponentByType<PostProcessOutput_NodeComponent>()->componentID, "Post Process"}};

        auto dumpNodeComponent = [](NodeComponent const& component) -> std::string {
            std::stringstream stream;
            stream << "{\"name\":\"" << component.getName() << "\",\"fixed\":" << std::boolalpha << component.isFixed()
                   << ",\"inputs\":[";

            bool isFirst = true;
            for (auto const& socket : component.setInputs())
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
            for (auto const& socket : component.setOutputs())
            {
                if (!isFirst)
                {
                    stream << ",";
                }

                stream << "{\"name\":\"" << socket.socketName << "\",\"type\":\"" << socket.socketType << "\"}";
                isFirst = false;
            }

            stream << "],\"userData\":{\"componentID\":" << component.componentID << ",\"options\":{";

            isFirst = true;
            for (auto const& [key, value] : component.setOptions())
            {
                if (!isFirst)
                {
                    stream << ",";
                }

                stream << "\"" << key << "\":\"" << value << "\"";
                isFirst = false;
            }

            stream << "}}}";
            return stream.str();
        };

        std::stringstream stream;
        stream << "{\"domains\":[";

        bool isFirst = true;
        for (auto const& [componentID, nodeComponent] : components)
        {
            if (!isFirst)
            {
                stream << ",";
            }

            stream << "{\"name\":\"" << shaderDomainNames[componentID]
                   << "\",\"node\":" << dumpNodeComponent(*nodeComponent) << "}";

            isFirst = false;
        }

        stream << "],\"inputNode\":" << dumpNodeComponent(*componentRegistry.getComponentByType<Input_NodeComponent>())
               << "}";
        return stream.str();
    }*/

    auto ViewModel::compileShader(std::string sceneData) -> std::string
    {
        shaderGraph = core::make_ref<Scene>(componentRegistry);

        std::cout << sceneData << std::endl;

        simdjson::ondemand::parser parser;
        auto document = parser.iterate(sceneData, sceneData.size() + simdjson::SIMDJSON_PADDING);

        std::unordered_map<uint64_t, core::ref_ptr<Node>> componentsOfNodes;

        for (auto node : document["nodes"])
        {
            uint64_t nodeID;
            auto error = node["id"].get_uint64().get(nodeID);
            if (error != simdjson::SUCCESS)
            {
                return "";
            }

            simdjson::ondemand::object userData;
            error = node["userData"].get_object().get(userData);
            if (error != simdjson::SUCCESS)
            {
                return "";
            }

            uint64_t componentID;
            error = userData["componentID"].get_uint64().get(componentID);
            if (error != simdjson::SUCCESS)
            {
                return "";
            }

            auto createdNode = shaderGraph->createNodeByID(componentID, nodeID);
            componentsOfNodes.emplace(nodeID, createdNode);

            simdjson::ondemand::object options;
            userData["options"].get_object().get(options);
            if (error != simdjson::SUCCESS)
            {
                return "";
            }

            for (auto element : options)
            {
                std::string_view key;
                error = element.unescaped_key().get(key);
                if (error != simdjson::SUCCESS)
                {
                    return "";
                }

                std::string_view value;
                error = element.value().get_string().get(value);
                if (error != simdjson::SUCCESS)
                {
                    return "";
                }

                createdNode->options[std::string(key)] = std::string(value);
            }

            if (createdNode->inputs.empty())
            {
                for (auto input : node["inputs"])
                {
                    std::string_view socketType;
                    error = input["type"].get_string().get(socketType);
                    if (error != simdjson::SUCCESS)
                    {
                        return "";
                    }

                    std::string_view socketName;
                    error = input["name"].get_string().get(socketName);
                    if (error != simdjson::SUCCESS)
                    {
                        return "";
                    }

                    Node::SocketInfo socketInfo = {.socketName = std::string(socketName),
                                                   .socketType = std::string(socketType)};

                    createdNode->inputs.emplace_back(std::move(socketInfo));
                }
            }

            if (createdNode->outputs.empty())
            {
                for (auto output : node["outputs"])
                {
                    std::string_view socketType;
                    error = output["type"].get_string().get(socketType);
                    if (error != simdjson::SUCCESS)
                    {
                        return "";
                    }

                    std::string_view socketName;
                    error = output["name"].get_string().get(socketName);
                    if (error != simdjson::SUCCESS)
                    {
                        return "";
                    }

                    Node::SocketInfo socketInfo = {.socketName = std::string(socketName),
                                                   .socketType = std::string(socketType)};

                    createdNode->outputs.emplace_back(std::move(socketInfo));
                }
            }
        }

        for (auto connection : document["connections"])
        {
            uint64_t connectionID;
            auto error = connection["id"].get_uint64().get(connectionID);
            if (error != simdjson::SUCCESS)
            {
                return "";
            }

            uint64_t sourceNodeID;
            error = connection["source"].get_uint64().get(sourceNodeID);
            if (error != simdjson::SUCCESS)
            {
                return "";
            }

            uint64_t destNodeID;
            error = connection["dest"].get_uint64().get(destNodeID);
            if (error != simdjson::SUCCESS)
            {
                return "";
            }

            uint64_t sourceIndex;
            error = connection["out"].get_uint64().get(sourceIndex);
            if (error != simdjson::SUCCESS)
            {
                return "";
            }

            uint64_t destIndex;
            error = connection["in"].get_uint64().get(destIndex);
            if (error != simdjson::SUCCESS)
            {
                return "";
            }

            shaderGraph->createConnection(connectionID, componentsOfNodes[sourceNodeID], sourceIndex,
                                          componentsOfNodes[destNodeID], destIndex);
        }

        this->compileShaderGraph();
        return "test";
    }

    auto ViewModel::compileShaderGraph() -> bool
    {
        std::stringstream shaderCodeStream;
        shaderGraph->bfs(shaderCodeStream);

        std::cout << shaderCodeStream.str() << std::endl;

        std::filesystem::path const appDataPath = std::getenv("APPDATA");

        if (!std::filesystem::exists(appDataPath / "ionengine-tools" / "Cache"))
        {
            if (!std::filesystem::create_directory(appDataPath / "ionengine-tools" / "Cache"))
            {
                throw core::Exception("An error occurred while creating the directory");
            }
        }

        std::string const fileName = std::filesystem::path(std::tmpnam(nullptr)).filename().string();

        {
            std::fstream stream(appDataPath / "ionengine-tools" / "Cache" / fileName, std::ios::out);
            if (!stream.is_open())
            {
                throw core::Exception("An error occurred while creating the file");
            }
            stream << shaderCodeStream.str();
        }

        std::string const outputName = std::filesystem::path(std::tmpnam(nullptr)).filename().string();

        std::string const inputPath = (appDataPath / "ionengine-tools" / "Cache" / fileName).generic_string();
        std::string const outputPath = (appDataPath / "ionengine-tools" / "Cache" / outputName).generic_string();

        std::vector<std::string> arguments = {"shaderc.exe", "\"" + inputPath + "\"", "--target", "DXIL",
                                              "--output",    "\"" + outputPath + "\""};

        auto result = core::subprocessExecute(arguments);

        if (result.has_value())
        {
            auto buffer = std::move(result.value());
            std::string const outputMessage(reinterpret_cast<char*>(buffer.data()), buffer.size());

            if (outputMessage.contains(std::format("Out: {}", outputPath)))
            {
                previewShader = this->createShaderAsset();

                if (previewShader->loadFromFile(outputPath))
                {
                    std::cout << "Shader Loaded" << std::endl;
                    renderer.registerShader("genShader", previewShader);

                    std::filesystem::remove(inputPath);
                    std::filesystem::remove(outputPath);

                    for (auto const [optionName, option] : previewShader->getOptions())
                    {
                        std::cout << std::format("{} {}", optionName, option.constantIndex) << std::endl;
                    }
                }
                return true;
            }
            else
            {
                std::filesystem::remove(inputPath);
                std::filesystem::remove(outputPath);
                return false;
            }
        }
        else
        {
            return false;
        }
    }
} // namespace ionengine::tools::editor