// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "gui.hpp"
#include "precompiled.h"
#include <RmlUi/Core.h>
#include <RmlUi/Lua.h>

namespace ionengine
{
    GUI::GUI(core::ref_ptr<rhi::RHI> RHI)
        : rmlSystem(std::make_unique<internal::RmlSystem>()), rmlRender(std::make_unique<internal::RmlRender>(RHI)),
          outputWidth(800), outputHeight(600)
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

    auto GUI::onResize(uint32_t const width, uint32_t const height) -> void
    {
        for (auto& [rmlContext, guiContext] : rmlRender->guiContexts)
        {
            // TODO! Render To Texture UI should not change self dimensions
            rmlContext->SetDimensions(Rml::Vector2i(width, height));
        }
    }

    auto GUI::createWidget(std::filesystem::path const& filePath) -> core::ref_ptr<GUIWidget>
    {
        Rml::Context* rmlContext = Rml::CreateContext(Rml::String(filePath.generic_string()),
                                                      Rml::Vector2i(instance->outputWidth, instance->outputHeight));
        auto guiWidget = core::make_ref<GUIWidget>(*instance->rmlRender, rmlContext, filePath);
        instance->rmlRender->guiContexts[rmlContext] = guiWidget;
        return guiWidget;
    }
} // namespace ionengine