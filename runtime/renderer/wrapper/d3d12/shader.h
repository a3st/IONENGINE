// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::wrapper {

class D3DShader : public Shader {
public:

    D3DShader(const ShaderType shader_type, const std::vector<byte>& blob) 
        : m_device(device), m_type(shader_type), m_blob(blob) {
        
        m_d3d12_shader.pShaderBytecode = m_blob.data();
        m_d3d12_shader.BytecodeLength = m_blob.size();
    }

    ShaderType get_type() const override { return m_shader_type; }

    const D3D12_SHADER_BYTECODE& get_d3d12_shader_bytecode() { return m_d3d12_shader; }

private:

    D3D12_SHADER_BYTECODE m_d3d12_shader;
    std::vector<byte> m_blob;
    ShaderType m_type;
};

}