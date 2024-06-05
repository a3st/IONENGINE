// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "view_model.hpp"
#include "engine/renderer/shader.hpp"
#include "precompiled.h"
#include <base64pp/base64pp.h>

#include "components/convert.hpp"
#include "components/input.hpp"
#include "components/output.hpp"
#include "components/texture.hpp"

namespace ionengine::tools::editor
{
    ViewModel::ViewModel(libwebview::App* app) : Engine(nullptr), app(app)
    {
        componentRegistry.registerComponent<Sampler2D_NodeComponent>();
        componentRegistry.registerComponent<Split_F4_F3F1_NodeComponent>();
        componentRegistry.registerComponent<PostProcessOutput_NodeComponent>();
        componentRegistry.registerComponent<Input_NodeComponent>();

        app->bind("addContextItems", [this]() -> std::string { return this->addContextItems(); });
        app->bind("addResourceTypes", [this]() -> std::string { return this->addResourceTypes(); });
        app->bind("addShaderDomains", [this]() -> std::string { return this->addShaderDomains(); });
        app->bind("requestPreviewImage", [this]() -> std::string { return this->requestPreviewImage(); });
    }

    auto ViewModel::init() -> void
    {
        core::ref_ptr<ShaderAsset> shaderAsset = this->createShaderAsset();
        bool result = shaderAsset->loadFromFile("../assets/shaders/quad.bin");
        std::cout << std::boolalpha << result << std::endl;

        for (auto const [optionName, option] : shaderAsset->getOptions())
        {
            std::cout << std::format("{} {}", optionName, option.constantIndex) << std::endl;
        }

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
        renderer.endDraw();
    }

    auto ViewModel::requestPreviewImage() -> std::string
    {
        return base64pp::encode(previewImage->dump());
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
            stream << "]}";
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

    auto ViewModel::addShaderDomains() -> std::string
    {
        auto components = componentRegistry.getComponents() | std::views::filter([](auto const& element) {
                              return element.second->isFixed() && element.second->getName().compare("Output Node") == 0;
                          });

        std::unordered_map<uint32_t, std::string> shaderDomainNames = {
            {componentRegistry.getComponentByType<PostProcessOutput_NodeComponent>()->componentID, "Post Process"}};

        std::stringstream stream;
        stream << "{\"domains\":[";

        bool isFirst = true;
        for (auto const& [componentID, nodeComponent] : components)
        {
            if (!isFirst)
            {
                stream << ",";
            }

            stream << "{\"name\":\"" << shaderDomainNames[componentID] << "\",\"node\":{\"name\":\""
                   << nodeComponent->getName() << "\",\"fixed\":" << nodeComponent->isFixed() << ",\"inputs\":[";

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
            stream << "]}}";
            isFirst = false;
        }
        stream << "]}";
        return stream.str();
    }
} // namespace ionengine::tools::editor