// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "gui_widget.hpp"
#include "precompiled.h"

namespace ionengine
{
    GUIWidget::GUIWidget()
    {
    }

    auto GUIWidget::getTargetCamera() const -> core::ref_ptr<Camera>
    {
        return targetCamera;
    }

    auto GUIWidget::attachToCamera(core::ref_ptr<Camera> targetCamera) -> void
    {
        this->targetCamera = targetCamera;
    }
} // namespace ionengine