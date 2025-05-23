// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/matrix.hpp"
#include "core/plane.hpp"
#include "core/quaternion.hpp"
#include "core/ref_ptr.hpp"
#include "image.hpp"
#include "render_queue.hpp"

namespace ionengine
{
    class Camera : public core::ref_counted_object
    {
      public:
        Camera();

        ~Camera() = default;

        virtual auto setViewMatrix(core::Mat4f const& viewMatrix) -> void;

        virtual auto getViewMatrix() const -> core::Mat4f const&;

        virtual auto getProjMatrix() -> core::Mat4f const&;

        virtual auto getTargetImage() const -> core::ref_ptr<RTImage>;

        virtual auto setTargetImage(core::ref_ptr<RTImage> const& targetImage) -> void;

        virtual auto customTargetImage() const -> bool;

        std::unordered_map<RenderGroup, RenderQueue> renderGroups;

      protected:
        core::Mat4f viewMatrix;
        core::Mat4f projMatrix;
        core::ref_ptr<RTImage> targetImage;
        bool hasRenderTarget;
        bool isNeedUpdate;

        virtual auto updateMatrices() -> void = 0;
    };

    class PerspectiveCamera : public Camera
    {
      public:
        PerspectiveCamera(float const fovy, float const aspect, float const zNear, float const zFar);

        auto createFrustumPlanes() const -> std::array<core::Planef, 6>;

        auto setAspect(float const aspect) -> void;

      private:
        float zNear;
        float zFar;
        float fovy;
        float aspect;

      protected:
        auto updateMatrices() -> void override;
    };
} // namespace ionengine