// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "graphics/camera.hpp"

namespace ionengine
{
    class GUIWidget : public core::ref_counted_object
    {
      public:
        GUIWidget();

        auto getTargetCamera() const -> core::ref_ptr<Camera>;

        auto attachToCamera(core::ref_ptr<Camera> targetCamera) -> void;

      private:
        core::ref_ptr<Camera> targetCamera;
    };
} // namespace ionengine