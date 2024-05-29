// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "app.hpp"
#include "components/sampler2d.hpp"
#include "precompiled.h"

namespace ionengine::tools::editor
{
    App::App(int32_t argc, char** argv) : app("ionengine-tools", "IONENGINE Editor", 800, 600, true, true)
    {
        this->registerComponents();
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
    }
} // namespace ionengine::tools::editor