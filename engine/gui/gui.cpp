// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "gui.hpp"
#include "precompiled.h"
#include <RmlUi/Core.h>

namespace ionengine
{
    GUI::GUI(core::ref_ptr<rhi::RHI> RHI)
        : rmlSystem(std::make_unique<internal::RmlSystem>()), rmlRender(std::make_unique<internal::RmlRender>(RHI))
    {
        instance = this;

        Rml::SetSystemInterface(rmlSystem.get());
        Rml::SetRenderInterface(rmlRender.get());
        Rml::Initialise();

        Rml::LoadFontFace("../../assets/fonts/Roboto-Regular.ttf");
    }

    GUI::~GUI()
    {
        Rml::Shutdown();
    }

    auto GUI::onRender() -> void
    {
        for (auto& [rmlContext, guiContext] : rmlRender->guiContexts)
        {
            rmlContext->Render();
        }
    }

    auto GUI::onUpdate() -> void
    {
        for (auto& [rmlContext, guiContext] : rmlRender->guiContexts)
        {
            rmlContext->Update();
        }
    }

    auto GUI::createWidgetFromFile(std::filesystem::path const& filePath) -> core::ref_ptr<GUIWidget>
    {
        Rml::Context* rmlContext = Rml::CreateContext(Rml::String(filePath.generic_string()), Rml::Vector2i(800, 600));
        return core::make_ref<GUIWidget>(*instance->rmlRender, rmlContext, filePath);
    }
} // namespace ionengine