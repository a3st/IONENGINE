// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "linked_device.hpp"
#include "core/exception.hpp"
#include "platform/window.hpp"
#include "precompiled.h"

namespace ionengine
{
    LinkedDevice::LinkedDevice(platform::Window* window)
    {
        rhi::RHICreateInfo rhiCreateInfo = {.numStaticSamplers = 0};

        if (window)
        {
            rhiCreateInfo.targetWindow = window->get_native_handle();
            rhiCreateInfo.windowWidth = window->get_width();
            rhiCreateInfo.windowHeight = window->get_height();

            device = rhi::Device::create(rhiCreateInfo);
            isWindowedRendering = true;
        }
        else
        {
            device = rhi::Device::create(rhiCreateInfo);
            isWindowedRendering = false;
        }

        graphicsContext = device->createGraphicsContext();
        copyContext = device->createCopyContext();
    }

    auto LinkedDevice::getDevice() -> rhi::Device&
    {
        return *device;
    }

    auto LinkedDevice::getGraphicsContext() -> rhi::GraphicsContext&
    {
        return *graphicsContext;
    }

    auto LinkedDevice::getCopyContext() -> rhi::CopyContext&
    {
        return *copyContext;
    }

    auto LinkedDevice::getBackBuffer() const -> core::ref_ptr<rhi::Texture>
    {
        return backBuffer;
    }

    auto LinkedDevice::beginFrame() -> void
    {
        graphicsContext->reset();

        if (isWindowedRendering)
        {
            backBuffer = device->requestBackBuffer();
        }
    }

    auto LinkedDevice::endFrame() -> void
    {
        rhi::Future<rhi::Query> graphics_future = graphicsContext->execute();

        if (isWindowedRendering)
        {
            device->presentBackBuffer();
        }

        graphics_future.wait();
    }

    auto LinkedDevice::beginUpload() -> void
    {
        copyContext->reset();
    }

    auto LinkedDevice::endUpload() -> void
    {
        rhi::Future<rhi::Query> copyFuture = copyContext->execute();
        copyFuture.wait();
    }

    auto LinkedDevice::uploadBuffer(core::ref_ptr<rhi::Buffer> buffer, std::span<uint8_t const> const data) -> void
    {
        copyContext->barrier(buffer, rhi::ResourceState::Common, rhi::ResourceState::CopyDst);
        copyContext->writeBuffer(buffer, data);
        copyContext->barrier(buffer, rhi::ResourceState::CopyDst, rhi::ResourceState::Common);
    }

    auto LinkedDevice::uploadTexture(core::ref_ptr<rhi::Texture> texture,
                                     std::vector<std::span<uint8_t const>> const& data) -> void
    {
        copyContext->barrier(texture, rhi::ResourceState::Common, rhi::ResourceState::CopyDst);
        copyContext->writeTexture(texture, data);
        copyContext->barrier(texture, rhi::ResourceState::CopyDst, rhi::ResourceState::Common);
    }

    auto LinkedDevice::readTexture(core::ref_ptr<rhi::Texture> texture, std::vector<std::vector<uint8_t>>& data) -> void
    {
        copyContext->barrier(texture, rhi::ResourceState::Common, rhi::ResourceState::CopySrc);
        copyContext->readTexture(texture, data);
        copyContext->barrier(texture, rhi::ResourceState::CopySrc, rhi::ResourceState::Common);
    }
} // namespace ionengine