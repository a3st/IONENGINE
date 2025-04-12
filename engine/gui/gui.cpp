// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "gui.hpp"
#include "precompiled.h"
#include <RmlUi/Core.h>

namespace ionengine
{
    GUI::GUI(core::ref_ptr<rhi::RHI> RHI)
        : rmlRender(std::make_unique<internal::RmlRender>(RHI)), rmlSystem(std::make_unique<internal::RmlSystem>())
    {
        instance = this;

        Rml::SetRenderInterface(rmlRender.get());
        Rml::SetSystemInterface(rmlSystem.get());
        Rml::Initialise();
    }

    auto GUI::createWidgetFromFile(std::filesystem::path const& filePath) -> core::ref_ptr<GUIWidget>
    {
        Rml::Context* rmlContext = Rml::CreateContext("_", Rml::Vector2i(1, 1));

        // auto guiContext = core::make_ref<GUIContext>(targetCamera);
        // instance->rmlRender->guiContexts[rmlContext] = guiContext;
        // return guiContext;
        return nullptr;
    }
} // namespace ionengine