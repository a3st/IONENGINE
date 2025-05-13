// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "camera.hpp"
#include "precompiled.h"

namespace ionengine
{
    Camera::Camera()
    {
        // Initialize Render Groups
        renderGroups.try_emplace(RenderGroup::Opaque);
        renderGroups.try_emplace(RenderGroup::Translucent);
        renderGroups.try_emplace(RenderGroup::UI);
    }

    auto Camera::setViewMatrix(core::Mat4f const& viewMatrix) -> void
    {
        this->viewMatrix = viewMatrix;
    }

    auto Camera::getViewMatrix() const -> core::Mat4f const&
    {
        return viewMatrix;
    }

    auto Camera::getProjMatrix() -> core::Mat4f const&
    {
        if (isNeedUpdate)
        {
            this->updateMatrices();
        }
        return projMatrix;
    }

    auto Camera::getTargetImage() const -> core::ref_ptr<RTImage>
    {
        return targetImage;
    }

    auto Camera::setTargetImage(core::ref_ptr<RTImage> const& targetImage) -> void
    {
        hasRenderTarget = true;
        this->targetImage = targetImage;
    }

    auto Camera::customTargetImage() const -> bool
    {
        return hasRenderTarget;
    }

    PerspectiveCamera::PerspectiveCamera(float const fovy, float const aspect, float const zNear, float const zFar)
        : fovy(fovy * std::numbers::pi / 180.0f), aspect(aspect), zFar(zFar), zNear(zNear)
    {
        this->isNeedUpdate = true;
        this->hasRenderTarget = false;
    }

    auto PerspectiveCamera::createFrustumPlanes() const -> std::array<core::Planef, 6>
    {
        float const halfVSide = zFar * std::tanf(fovy * 0.5f);
        float const halfHSide = halfVSide * aspect;

        // core::Planef
        return {};
    }

    auto PerspectiveCamera::setAspect(float const aspect) -> void
    {
        this->aspect = aspect;
        isNeedUpdate = true;
    }

    auto PerspectiveCamera::updateMatrices() -> void
    {
        projMatrix = core::Mat4f::perspectiveRH(fovy, aspect, zNear, zFar);
        isNeedUpdate = false;
    }

    /*auto PerspectiveCamera::updateDirectionVectors() -> void
    {
        core::Mat4f const rotMat = rotation.toMat4();
        forward = core::Vec3f(rotMat._00, rotMat._10, rotMat._20).normalize();
        right = forward.cross(core::Vec3f(0.0f, 0.0f, 1.0f)).normalize();
        up = right.cross(forward).normalize();
    }*/
} // namespace ionengine