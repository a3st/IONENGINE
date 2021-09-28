// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::api {

class D3DShader : public Shader {
public:

    D3DShader(winrt::com_ptr<ID3D12Device4>& device, const ShaderType shader_type, const std::vector<byte>& shader_blob) 
        : m_device(device), m_shader_type(shader_type), m_shader_blob(shader_blob) {
        
        m_d3d12_shader.pShaderBytecode = m_shader_blob.data();
        m_d3d12_shader.BytecodeLength = m_shader_blob.size();
    }

    ShaderType get_type() const { return m_shader_type; }

    const D3D12_SHADER_BYTECODE& get_d3d12_shader() { return m_d3d12_shader; }

private:

    std::reference_wrapper<winrt::com_ptr<ID3D12Device4>> m_device;

    D3D12_SHADER_BYTECODE m_d3d12_shader;
    std::vector<byte> m_shader_blob;

    ShaderType m_shader_type;
};

}