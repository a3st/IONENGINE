// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "app.hpp"
#include "components/output.hpp"
#include "components/sampler2d.hpp"
#include "precompiled.h"

namespace ionengine::tools::editor
{
    App::App(int32_t argc, char** argv) : app("ionengine-tools", "IONENGINE Editor", 800, 600, true, true)
    {
        this->registerComponents();

        std::vector<std::string> resourceTypes = {"Texture2D", "float4", "float3", "float2", "float", "bool"};

        app.bind("addResourceTypes", [&, resourceTypes](libwebview::EventArgs const& e) {
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
            app.result(e.index, true, stream.str());
        });
    }

    auto App::run() -> int32_t
    {
        if (app.run("resources/index.html"))
        {
            return EXIT_SUCCESS;
        }
        else
        {
            return EXIT_FAILURE;
        }
    }

    auto App::registerComponents() -> void
    {
        registry.registerComponent<editor::Sampler2DNode>("Texture");

        app.bind("addContextItems",
                 [&](libwebview::EventArgs const& e) { app.result(e.index, true, registry.dump()); });

        merger.registerComponent<editor::SurfaceOutputNode>("Surface");
        merger.registerComponent<editor::PostProcessOutputNode>("Post Process");

        app.bind("addShaderDomains", [&](libwebview::EventArgs const& e) { app.result(e.index, true, merger.dump()); });
    }
} // namespace ionengine::tools::editor