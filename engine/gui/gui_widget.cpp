// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "gui_widget.hpp"
#include "precompiled.h"

namespace ionengine
{
    GUIWidget::GUIWidget(core::ref_ptr<Camera> targetCamera) : targetCamera(targetCamera)
    {
    }

    auto GUIWidget::getTargetCamera() const -> core::ref_ptr<Camera>
    {
        return nullptr;
    }

    auto GUIWidget::attachToCamera(core::ref_ptr<Camera> targetCamera) -> void
    {
    }
} // namespace ionengine