// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "linked_device.hpp"
#include "core/exception.hpp"
#include "platform/window.hpp"
#include "precompiled.h"

namespace ionengine
{
    LinkedDevice::LinkedDevice(platform::Window* window)
    {
        if (window)
        {
            device = rhi::Device::create(window);
            isWindowedRendering = true;
        }
        else
        {
            device = rhi::Device::create(nullptr);
            isWindowedRendering = false;
        }

        graphicsContext = device->create_graphics_context();
        copyContext = device->create_copy_context();
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
            backBuffer = device->request_back_buffer();
        }
    }

    auto LinkedDevice::endFrame() -> void
    {
        rhi::Future<rhi::Query> graphics_future = graphicsContext->execute();

        if (isWindowedRendering)
        {
            device->present_back_buffer();
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
        copyContext->write_buffer(buffer, data);
        copyContext->barrier(buffer, rhi::ResourceState::CopyDst, rhi::ResourceState::Common);
    }

    auto LinkedDevice::uploadTexture(core::ref_ptr<rhi::Texture> texture,
                                     std::vector<std::span<uint8_t const>> const& data) -> void
    {
        copyContext->barrier(texture, rhi::ResourceState::Common, rhi::ResourceState::CopyDst);
        copyContext->write_texture(texture, data);
        copyContext->barrier(texture, rhi::ResourceState::CopyDst, rhi::ResourceState::Common);
    }

    auto LinkedDevice::readTexture(core::ref_ptr<rhi::Texture> texture, std::vector<std::vector<uint8_t>>& data) -> void
    {
        copyContext->barrier(texture, rhi::ResourceState::Common, rhi::ResourceState::CopySrc);
        copyContext->read_texture(texture, data);
        copyContext->barrier(texture, rhi::ResourceState::CopySrc, rhi::ResourceState::Common);
    }
} // namespace ionengine