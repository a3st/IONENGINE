// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/matrix.hpp"
#include "core/plane.hpp"
#include "core/quaternion.hpp"
#include "core/ref_ptr.hpp"

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

        auto setFieldOfView(float const value) -> void;

        auto setFarClipPlane(float const value) -> void;

        auto setNearClipPlane(float const value) -> void;

        auto setAspect(float const value) -> void;

        auto getViewType() const -> CameraViewType;

        auto setPosition(core::Vec3f const& position) -> void;

        auto calculateProjMatrix() const -> core::Mat4f;

        auto calculateViewMatrix() const -> core::Mat4f;

        auto createFrustumPlanes() const -> std::array<core::Planef, 6>;

      private:
        core::Vec3f position;
        core::Quatf rotation;

        core::Vec3f up;
        core::Vec3f right;
        core::Vec3f forward;

        float zNear;
        float zFar;
        float fovy;
        float aspect;

        CameraViewType viewType;

        auto updateDirectionVectors() -> void;
    };
} // namespace ionengine