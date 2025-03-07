// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/matrix.hpp"
#include "rhi/rhi.hpp"

namespace ionengine
{
    enum class CameraViewType
    {
        Perspective,
        Orthographic
    };

    class Camera : public core::ref_counted_object
    {
      public:
        Camera(CameraViewType const viewType);

        auto isMainCamera() const -> bool;

        auto getViewType() const -> CameraViewType;

      private:
        core::Mat4f projMat;
        core::Mat4f viewMat;

        CameraViewType viewType;
        bool mainCamera;
    };
} // namespace ionengine