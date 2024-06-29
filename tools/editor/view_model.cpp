// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "view_model.hpp"
#include "core/exception.hpp"
#include "engine/renderer/shader.hpp"
#include "precompiled.h"
#include <base64pp/base64pp.h>

#include "shader_graph/components/convert.hpp"
#include "shader_graph/components/input.hpp"
#include "shader_graph/components/math.hpp"
#include "shader_graph/components/output.hpp"
#include "shader_graph/components/texture.hpp"

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
        : Engine(nullptr), app(app), assetTree("E:\\GitHub\\IONENGINE\\build\\assets"),
          shaderGraphEditor(shaderComponentRegistry)
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

        shaderComponentRegistry.registerComponent<Input_NodeComponent>();
        shaderComponentRegistry.registerComponent<UnlitOutput_NodeComponent>();
        shaderComponentRegistry.registerComponent<Sampler2D_NodeComponent>();
        shaderComponentRegistry.registerComponent<Constant_Color_NodeComponent>();
        shaderComponentRegistry.registerComponent<Constant_Float_NodeComponent>();
        shaderComponentRegistry.registerComponent<Join_Float4_NodeComponent>();
        shaderComponentRegistry.registerComponent<Split_Float4_NodeComponent>();
        shaderComponentRegistry.registerComponent<Split_Float3_NodeComponent>();
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
            createPath = initialGenPath.make_preferred();

            if (fileInfo.assetType.compare("asset/shadergraph/unlit") == 0)
            {
                auto shaderGraphAsset = core::make_ref<ShaderGraphAsset>(shaderComponentRegistry);
                shaderGraphAsset->create(ShaderGraphType::Unlit);

                // Change to general shadergraph type
                fileInfo.assetType = "asset/shadergraph";

                ShaderGraphFile shaderGraphFile = {
                    .fileHeader = {.magic = asset::Magic, .fileType = ShaderGraphFileType},
                    .graphData = std::move(shaderGraphAsset->getGraphData())};

                if (!core::saveToFile<ShaderGraphFile>(shaderGraphFile, createPath))
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
            auto shaderGraphAsset = core::make_ref<ShaderGraphAsset>(shaderComponentRegistry);
            if (!shaderGraphAsset->loadFromFile(std::filesystem::path(fileInfo.assetPath).make_preferred()))
            {
                return "{\"error\":1}";
            }

            shaderGraphEditor.loadAsset(shaderGraphAsset);

            auto buffer = core::saveToBytes<ShaderGraphData, core::serialize::OutputJSON>(
                              shaderGraphEditor.getAsset()->getGraphData())
                              .value();
            return std::string(reinterpret_cast<char*>(buffer.data()), buffer.size());
        }
        else
        {
            return "{\"error\":2}";
        }
    }

    auto ViewModel::getShaderGraphComponents() -> std::string
    {
        auto components = shaderComponentRegistry.getComponents() |
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

        auto resultScene = core::loadFromBytes<ShaderGraphData, core::serialize::InputJSON>(
            std::span<uint8_t>(reinterpret_cast<uint8_t*>(sceneData.data()), sceneData.size()));
        if (!resultScene.has_value())
        {
            return "{\"error\":1}";
        }

        ShaderGraphData graphData = std::move(resultScene.value());

        ShaderGraphFile shaderGraphFile = {.fileHeader = {.magic = asset::Magic, .fileType = ShaderGraphFileType},
                                           .graphData = std::move(graphData)};

        if (!core::saveToFile<ShaderGraphFile>(shaderGraphFile,
                                               std::filesystem::path(fileInfo.assetPath).make_preferred()))
        {
            return "{\"error\":16}";
        }

        auto buffer = core::saveToBytes<AssetSaveFileInfo, core::serialize::OutputJSON>(fileInfo).value();
        return std::string(reinterpret_cast<char*>(buffer.data()), buffer.size());
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

        auto shaderGraphAsset = core::make_ref<ShaderGraphAsset>(shaderComponentRegistry);
        if (!shaderGraphAsset->loadFromFile(fileInfo.assetPath))
        {
            return "{\"error\":1}";
        }

        shaderGraphEditor.loadAsset(shaderGraphAsset);

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