// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "material.hpp"
#include "core/exception.hpp"
#include "linked_device.hpp"
#include "material.hpp"
#include "precompiled.h"
#include "shader_manager.hpp"
#include "texture.hpp"

namespace ionengine
{
    Material::Material(LinkedDevice& device, ShaderManager& shader_manager)
        : device(&device), shader_manager(&shader_manager)
    {
    }

    auto Material::load_from_memory(std::span<uint8_t const> const data) -> bool
    {
        return false;
    }

    auto Material::create_using_shader(std::string_view const shader_name) -> bool
    {
        core::ref_ptr<rhi::Shader> shader = shader_manager->get_shader_by_name(shader_name);

        auto result = shader->get_bindings().find("MaterialData");

        if (result == shader->get_bindings().end())
        {
            throw core::Exception("Shader incompatible with material system");
        }
        if (result->second.resource_type != rhi::shader_file::ResourceType::Uniform)
        {
            throw core::Exception("Shader incompatible with material system");
        }

        for (auto const& data : result->second.elements)
        {
            parameters.emplace(data.name, std::make_pair(data.offset, data.element_type));
        }

        buffer_data.resize(result->second.size);
        buffer = device->get_device().create_buffer(result->second.size, 0,
                                                    (rhi::BufferUsageFlags)rhi::BufferUsage::ConstantBuffer);
        return true;
    }

    auto Material::set_parameter_data(std::string_view const param, void* data, size_t const size) -> void
    {
        auto result = parameters.find(std::string(param));

        if (result == parameters.end())
        {
            throw core::Exception("Invalid parameter name passed to shader");
        }

        std::memcpy(buffer_data.data() + result->second.first, data, size);
    }

    auto Material::set_parameter_texture(std::string_view const param, Texture& texture) -> void
    {
        auto result = parameters.find(std::string(param));

        if (result == parameters.end())
        {
            throw core::Exception("Invalid parameter name passed to shader");
        }

        // std::memcpy(buffer_data.data() + result->second.first, texture., size);
    }

    auto Material::get_shader() const -> core::ref_ptr<rhi::Shader>
    {
        return shader;
    }

    auto Material::get_buffer() -> core::ref_ptr<rhi::Buffer>
    {
        return buffer;
    }
} // namespace ionengine