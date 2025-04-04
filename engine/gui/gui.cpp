// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "gui.hpp"
#include "precompiled.h"
#include <RmlUi/Core.h>

namespace ionengine
{
    GUI::GUI(core::ref_ptr<rhi::RHI> RHI) : rmlRender(std::make_unique<internal::RmlRender>(RHI))
    {
        instance = this;

        Rml::SetRenderInterface(rmlRender.get());

        Rml::Initialise();
    }

    auto GUI::createContext(core::ref_ptr<Camera> targetCamera) -> core::ref_ptr<GUIContext>
    {
        Rml::Context* rmlContext = Rml::CreateContext(
            "_", Rml::Vector2i(targetCamera->getTexture()->getWidth(), targetCamera->getTexture()->getHeight()));

        auto guiContext = core::make_ref<GUIContext>(targetCamera);
        rmlRender->guiContexts[rmlContext] = guiContext;
        return guiContext;
    }
} // namespace ionengine