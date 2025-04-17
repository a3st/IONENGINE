// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "gui_widget.hpp"
#include "precompiled.h"
#include "rml.hpp"

namespace ionengine
{
    GUIWidget::GUIWidget(internal::RmlRender& rmlRender, Rml::Context* context, std::filesystem::path const& filePath)
        : rmlRender(&rmlRender), context(context), filePath(filePath)
    {
        rmlRender.guiContexts[context] = this;

        document = context->LoadDocument(Rml::String(filePath.generic_string()));
    }

    GUIWidget::~GUIWidget()
    {
        // rmlRender->guiContexts.erase(context);

        // Rml::RemoveContext(Rml::String(filePath.generic_string()));
    }

    auto GUIWidget::getTargetCamera() const -> core::ref_ptr<Camera>
    {
        return targetCamera;
    }

    auto GUIWidget::attachToCamera(core::ref_ptr<Camera> targetCamera) -> void
    {
        this->targetCamera = targetCamera;
        document->Show();
    }
} // namespace ionengine