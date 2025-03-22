// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/matrix.hpp"
#include "core/plane.hpp"
#include "core/quaternion.hpp"
#include "core/ref_ptr.hpp"
#include "rhi/rhi.hpp"

namespace ionengine
{
    class Camera : public core::ref_counted_object
    {
      public:
        ~Camera() = default;

        virtual auto setViewMatrix(core::Mat4f const& viewMatrix) -> void;

        virtual auto getViewMatrix() const -> core::Mat4f const&;

        virtual auto getProjMatrix() const -> core::Mat4f const&;

        virtual auto getTexture() const -> core::ref_ptr<rhi::Texture>;

      protected:
        core::Mat4f viewMat;
        core::Mat4f projMat;
        core::ref_ptr<rhi::Texture> targetTexture;
    };

    class PerspectiveCamera : public Camera
    {
      public:
        PerspectiveCamera(rhi::RHI& RHI, float const fovy, float const zNear, float const zFar);

        auto createFrustumPlanes() const -> std::array<core::Planef, 6>;

      private:
        float zNear;
        float zFar;
        float fovy;
        float aspect;
    };
} // namespace ionengine