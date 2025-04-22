// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/matrix.hpp"
#include "core/plane.hpp"
#include "core/quaternion.hpp"
#include "core/ref_ptr.hpp"
#include "image.hpp"

namespace ionengine
{
    class Camera : public core::ref_counted_object
    {
      public:
        ~Camera() = default;

        virtual auto setViewMatrix(core::Mat4f const& viewMatrix) -> void;

        virtual auto getViewMatrix() const -> core::Mat4f const&;

        virtual auto getProjMatrix() const -> core::Mat4f const&;

        virtual auto getTargetImage() const -> core::ref_ptr<RTImage>;

        virtual auto setTargetImage(core::ref_ptr<RTImage> const& targetImage) -> void;

        virtual auto customTargetImage() const -> bool;

      protected:
        core::Mat4f viewMatrix;
        core::Mat4f projMatrix;
        core::ref_ptr<RTImage> targetImage;
        bool hasRenderTarget;
    };

    class PerspectiveCamera : public Camera
    {
      public:
        PerspectiveCamera(float const fovy, float const aspect, float const zNear, float const zFar);

        auto createFrustumPlanes() const -> std::array<core::Planef, 6>;

      private:
        float zNear;
        float zFar;
        float fovy;
        float aspect;
    };
} // namespace ionengine