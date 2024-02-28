
#include "linked_device.hpp"
#include "precompiled.h"

namespace ionengine
{
    LinkedDevice::LinkedDevice(platform::Window* window)
    {
        if (window)
        {
            device = rhi::Device::create(window);
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

    auto LinkedDevice::begin_frame() -> void
    {
        graphics_context->reset();
        if (is_windowed_rendering)
        {
            frame_texture = device->request_back_buffer();
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
        std::lock_guard lock(mutex);

        if (!copy_future.get_result())
        {
            copy_future.wait();
        }

        copy_context->reset();
    }

    auto LinkedDevice::end_upload() -> void
    {
        std::lock_guard lock(mutex);

        copy_future = copy_context->execute();
    }

    auto LinkedDevice::upload(core::ref_ptr<rhi::Buffer> buffer, std::span<uint8_t const> const data) -> void
    {
        std::lock_guard lock(mutex);

        copy_context->barrier(buffer, rhi::ResourceState::Common, rhi::ResourceState::CopyDst);
        copy_context->write_buffer(buffer, data);
        copy_context->barrier(buffer, rhi::ResourceState::CopyDst, rhi::ResourceState::Common);
    }

    auto LinkedDevice::upload(core::ref_ptr<rhi::Texture> texture, std::vector<std::span<uint8_t const>> const& data)
        -> void
    {
        std::lock_guard lock(mutex);

        copy_context->barrier(texture, rhi::ResourceState::Common, rhi::ResourceState::CopyDst);
        copy_context->write_texture(texture, data);
        copy_context->barrier(texture, rhi::ResourceState::CopyDst, rhi::ResourceState::Common);
    }

    auto LinkedDevice::read(core::ref_ptr<rhi::Texture> texture, std::vector<std::vector<uint8_t>>& data) -> void
    {
        std::lock_guard lock(mutex);

        copy_context->barrier(texture, rhi::ResourceState::Common, rhi::ResourceState::CopySrc);
        copy_context->read_texture(texture, data);
        copy_context->barrier(texture, rhi::ResourceState::CopySrc, rhi::ResourceState::Common);
    }
} // namespace ionengine