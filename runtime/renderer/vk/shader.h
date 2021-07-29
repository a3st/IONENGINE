// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer
{

class Shader final {
public:

    Shader(Device& device, const ShaderType shader_type, const std::filesystem::path& file_path) : m_device(device), m_type(shader_type)  {

        // read a file
        {
            std::ifstream file(file_path, std::ios::ate | std::ios::binary);
            if (!file.is_open()) {
                throw std::runtime_error(format<std::string>("File cannot be opened '{}'", file_path));
            }

            usize file_size = static_cast<usize>(file.tellg());
            m_data.resize(file_size);

            file.seekg(std::ios::beg);
            file.read(m_data.data(), file_size);
        }

        vk::ShaderModuleCreateInfo shader_info{};

        shader_info
            .setPCode(reinterpret_cast<const uint32*>(m_data.data()))
            .setCodeSize(m_data.size());

        m_handle = m_device.get().get_handle()->createShaderModuleUnique(shader_info);
    }

    Shader(const Shader&) = delete;

    Shader(Shader&& rhs) noexcept : m_device(rhs.m_device) {

        m_handle.swap(rhs.m_handle);

        m_type = rhs.m_type;
        std::swap(m_data, rhs.m_data);
    }

    Shader& operator=(const Shader&) = delete;

    Shader& operator=(Shader&& rhs) {

        std::swap(m_device, rhs.m_device);

        m_handle.swap(rhs.m_handle);

        m_type = rhs.m_type;
        std::swap(m_data, rhs.m_data);
        return *this;
    }

    ShaderType get_type() const { return m_type; }
    const vk::UniqueShaderModule& get_handle() const { return m_handle; }

private:

    std::reference_wrapper<Device> m_device;

    vk::UniqueShaderModule m_handle;

    ShaderType m_type;
    std::vector<char> m_data;
};

}