// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "view_model.hpp"
#include "components/output.hpp"
#include "components/sampler2d.hpp"
#include "precompiled.h"
#include "engine/renderer/shader.hpp"

namespace ionengine::tools::editor
{
    ViewModel::ViewModel(libwebview::App* app) : Engine(nullptr), app(app)
    {
        componentRegistry.registerComponent<editor::Sampler2DNode>("Texture");

        app->bind("addContextItems", [this]() { return componentRegistry.dump(); });

        std::vector<std::string> resourceTypes = {"Texture2D", "float4", "float3", "float2", "float", "bool"};

        app->bind("addResourceTypes", [this, resourceTypes]() {
            std::stringstream stream;
            stream << "{\"types\":[";
            bool isFirst = true;

            for (auto const& resourceType : resourceTypes)
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
        });

        domainRegistry.registerDomain<editor::SurfaceOutputNode>("Surface");
        domainRegistry.registerDomain<editor::PostProcessOutputNode>("Post Process");

        app->bind("addShaderDomains", [this]() { return domainRegistry.dump(); });
    }

    auto ViewModel::init() -> void
    {
        core::ref_ptr<ShaderAsset> shaderAsset = this->createShaderAsset();
        bool result = shaderAsset->loadFromFile("../assets/shaders/quad.bin");
        std::cout << std::boolalpha << result << std::endl;
    }

    auto ViewModel::update(float const dt) -> void
    {
    }

    auto ViewModel::render() -> void
    {
        
    }
} // namespace ionengine::tools::editor