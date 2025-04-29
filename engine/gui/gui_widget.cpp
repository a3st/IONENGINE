// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "gui_widget.hpp"
#include "precompiled.h"
#include "rml.hpp"

namespace ionengine
{
    GUIWidget::GUIWidget(internal::RmlRender& rmlRender, Rml::Context* rmlContext,
                         std::filesystem::path const& filePath)
        : rmlRender(&rmlRender), rmlContext(rmlContext), filePath(filePath)
    {
        document = rmlContext->LoadDocument(Rml::String(filePath.generic_string()));
    }

    GUIWidget::~GUIWidget()
    {
        rmlRender->guiContexts.erase(rmlContext);
        Rml::RemoveContext(Rml::String(filePath.generic_string()));
    }

    auto GUIWidget::getTargetCamera() const -> core::ref_ptr<Camera>
    {
        return targetCamera;
    }

    auto GUIWidget::attachTo(core::ref_ptr<Camera> const& targetCamera) -> void
    {
        this->targetCamera = targetCamera;
        document->Show();
    }
} // namespace ionengine