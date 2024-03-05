// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "linked_device.hpp"
#include "core/exception.hpp"
#include "platform/window.hpp"
#include "precompiled.h"

namespace ionengine
{
    LinkedDevice::LinkedDevice(core::ref_ptr<platform::Window> window)
    {
        if (window)
        {
            device = rhi::Device::create(window.get());
            is_windowed_rendering = true;
        }
        else
        {
            device = rhi::Device::create(nullptr);
            is_windowed_rendering = false;
        }

        graphics_context = device->create_graphics_context();
        copy_context = device->create_copy_context();
    }

    auto LinkedDevice::get_device() -> rhi::Device&
    {
        return *device;
    }

    auto LinkedDevice::get_graphics_context() -> rhi::GraphicsContext&
    {
        return *graphics_context;
    }

    auto LinkedDevice::get_copy_context() -> rhi::CopyContext&
    {
        return *copy_context;
    }

    auto LinkedDevice::get_back_buffer() const -> core::ref_ptr<rhi::Texture>
    {
        return back_buffer;
    }

    auto LinkedDevice::begin_frame() -> void
    {
        graphics_context->reset();

        if (is_windowed_rendering)
        {
            back_buffer = device->request_back_buffer();
        }
    }

    auto LinkedDevice::end_frame() -> void
    {
        rhi::Future<rhi::Query> graphics_future = graphics_context->execute();

        if (is_windowed_rendering)
        {
            device->present_back_buffer();
        }

        graphics_future.wait();
    }

    auto LinkedDevice::begin_upload() -> void
    {
        copy_context->reset();
    }

    auto LinkedDevice::end_upload() -> void
    {
        rhi::Future<rhi::Query> copy_future = copy_context->execute();
        copy_future.wait();
    }

    auto LinkedDevice::upload(core::ref_ptr<rhi::Buffer> buffer, std::span<uint8_t const> const data) -> void
    {
        copy_context->barrier(buffer, rhi::ResourceState::Common, rhi::ResourceState::CopyDst);
        copy_context->write_buffer(buffer, data);
        copy_context->barrier(buffer, rhi::ResourceState::CopyDst, rhi::ResourceState::Common);
    }

    auto LinkedDevice::upload(core::ref_ptr<rhi::Texture> texture, std::vector<std::span<uint8_t const>> const& data)
        -> void
    {
        copy_context->barrier(texture, rhi::ResourceState::Common, rhi::ResourceState::CopyDst);
        copy_context->write_texture(texture, data);
        copy_context->barrier(texture, rhi::ResourceState::CopyDst, rhi::ResourceState::Common);
    }

    auto LinkedDevice::read(core::ref_ptr<rhi::Texture> texture, std::vector<std::vector<uint8_t>>& data) -> void
    {
        copy_context->barrier(texture, rhi::ResourceState::Common, rhi::ResourceState::CopySrc);
        copy_context->read_texture(texture, data);
        copy_context->barrier(texture, rhi::ResourceState::CopySrc, rhi::ResourceState::Common);
    }
} // namespace ionengine