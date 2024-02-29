
#include "linked_device.hpp"
#include "core/exception.hpp"
#include "platform/window.hpp"
#include "precompiled.h"

namespace ionengine
{
    LinkedDevice::LinkedDevice(std::span<std::filesystem::path const> const shader_paths, platform::Window* window)
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

        initialize_shaders(shader_paths);
    }

    auto LinkedDevice::initialize_shaders(std::span<std::filesystem::path const> const shader_paths) -> void
    {
        for (auto const& shader_path : shader_paths)
        {
            std::vector<uint8_t> data;
            {
                std::basic_ifstream<uint8_t> ifs(shader_path, std::ios::binary | std::ios::in);
                ifs.seekg(0, std::ios::end);
                size_t const size = ifs.tellg();
                ifs.seekg(0, std::ios::beg);
                data.resize(size);
                ifs.read(data.data(), data.size());
            }

            core::ref_ptr<rhi::Shader> shader = device->create_shader(data);

            auto result = shaders.find(std::string(shader->get_name()));

            if (result != shaders.end())
            {
                throw core::Exception("Cannot add an existing shader");
            }

            shaders.emplace(std::string(shader->get_name()), shader);
        }
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

    auto LinkedDevice::get_shader_by_name(std::string_view const shader_name) -> core::ref_ptr<rhi::Shader>
    {
        std::lock_guard lock(mutex);

        auto result = shaders.find(std::string(shader_name));

        if (result == shaders.end())
        {
            throw core::Exception("Can't get a shader that doesn't exist");
        }

        return result->second;
    }
} // namespace ionengine