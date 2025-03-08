// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "camera.hpp"
#include "precompiled.h"

namespace ionengine
{
    Camera::Camera(CameraViewType const viewType) : viewType(viewType)
    {
        this->updateDirectionVectors();
    }

    auto Camera::setFieldOfView(float const value) -> void
    {
        fovy = value * std::numbers::pi / 180.0f;
    }

    auto Camera::setFarClipPlane(float const value) -> void
    {
        zFar = value;
    }

    auto Camera::setNearClipPlane(float const value) -> void
    {
        zNear = value;
    }

    auto Camera::setAspect(float const value) -> void
    {
        aspect = value;
    }

    auto Camera::getViewType() const -> CameraViewType
    {
        return viewType;
    }

    auto Camera::setPosition(core::Vec3f const& position) -> void
    {
        this->position = position;
    }

    auto Camera::calculateProjMatrix() const -> core::Mat4f
    {
        return core::Mat4f::perspectiveRH(fovy, aspect, zNear, zFar);
    }

    auto Camera::calculateViewMatrix() const -> core::Mat4f
    {
        return core::Mat4f::lookAtRH(position, position + forward, up);
    }

    auto Camera::createFrustumPlanes() const -> std::array<core::Planef, 6>
    {
        float const halfVSide = zFar * std::tanf(fovy * 0.5f);
        float const halfHSide = halfVSide * aspect;

        // core::Planef
        return {};
    }

    auto Camera::updateDirectionVectors() -> void
    {
        core::Mat4f const rotMat = rotation.toMat4();
        forward = core::Vec3f(rotMat._00, rotMat._10, rotMat._20).normalize();
        right = forward.cross(core::Vec3f(0.0f, 0.0f, 1.0f)).normalize();
        up = right.cross(forward).normalize();
    }
} // namespace ionengine