// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

enum class ShaderType {
    Vertex = 0,
    Fragment = 1,
    Geometry = 2,
    Hull = 3,
    Domain = 4,
    Compute = 5
};


class Shader {
public:

    Shader(Device& device, const std::vector<byte>& shader_code) : m_shader_code(shader_code) {

        m_shader.pShaderBytecode = m_shader_code.data();
        m_shader.BytecodeLength = m_shader_code.size();
    }

    Shader(const Shader&) = delete;

    Shader(Shader&& rhs) noexcept {

        std::swap(m_shader, rhs.m_shader);
        std::swap(m_shader_code, rhs.m_shader_code);
    }

    Shader& operator=(const Shader&) = delete;

    Shader& operator=(Shader&& rhs) noexcept {

        std::swap(m_shader, rhs.m_shader);
        std::swap(m_shader_code, rhs.m_shader_code);
        return *this;
    }

private:

    D3D12_SHADER_BYTECODE m_shader;
    std::vector<byte> m_shader_code;
};

}