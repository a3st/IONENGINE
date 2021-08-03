// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class D3DShader : public Shader {
public:

    D3DShader(const ComPtr<ID3D12Device4>& device, const std::vector<byte>& blob) : m_blob(blob) {
        
        m_d3d12_shader.pShaderBytecode = m_blob.data();
        m_d3d12_shader.BytecodeLength = m_blob.size();
    }

    const D3D12_SHADER_BYTECODE& get_shader() { return m_d3d12_shader; }

private:

    D3D12_SHADER_BYTECODE m_d3d12_shader;
    std::vector<byte> m_blob;
};

}