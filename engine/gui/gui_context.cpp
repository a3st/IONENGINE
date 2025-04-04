// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "gui_context.hpp"
#include "precompiled.h"

namespace ionengine
{
    GUIContext::GUIContext(core::ref_ptr<Camera> targetCamera) : targetCamera(targetCamera)
    {
    }

    auto GUIContext::getTargetCamera() const -> core::ref_ptr<Camera>
    {
        return nullptr;
    }
} // namespace ionengine