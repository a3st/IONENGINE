// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "shader_manager.hpp"
#include "core/exception.hpp"
#include "linked_device.hpp"
#include "precompiled.h"

namespace ionengine
{
    ShaderManager::ShaderManager(LinkedDevice& device) : device(&device)
    {
    }

    auto ShaderManager::load_shader(std::filesystem::path const& shader_path) -> void
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

        core::ref_ptr<rhi::Shader> shader = device->get_device().create_shader(data);

        auto result = shaders.find(std::string(shader->get_name()));

        if (result != shaders.end())
        {
            throw core::Exception("Cannot add an existing shader");
        }

        shaders.emplace(std::string(shader->get_name()), shader);
    }

    auto ShaderManager::get_shader_by_name(std::string_view const shader_name) -> core::ref_ptr<rhi::Shader>
    {
        auto result = shaders.find(std::string(shader_name));

        if (result == shaders.end())
        {
            throw core::Exception("Can't get a shader that doesn't exist");
        }

        return result->second;
    }
} // namespace ionengine