// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "camera.hpp"
#include "precompiled.h"

namespace ionengine
{
    Camera::Camera(CameraViewType const viewType)
    {
    }

    auto Camera::isMainCamera() const -> bool
    {
        return mainCamera;
    }

    auto Camera::getViewType() const -> CameraViewType
    {
        return viewType;
    }
} // namespace ionengine