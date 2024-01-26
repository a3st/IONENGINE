// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "shader.hpp"
#include <xxhash/xxhash64.h>

using namespace ionengine;
using namespace ionengine::renderer;
using namespace ionengine::renderer::rhi;

auto ionengine::renderer::rhi::element_type_to_dxgi(shader_file::ElementType const element_type) -> DXGI_FORMAT {

    switch(element_type) {
        case shader_file::ElementType::Float4:
        case shader_file::ElementType::Float4x4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case shader_file::ElementType::Float3:
        case shader_file::ElementType::Float3x3: return DXGI_FORMAT_R32G32B32_FLOAT;
        case shader_file::ElementType::Float2:
        case shader_file::ElementType::Float2x2: return DXGI_FORMAT_R32G32_FLOAT;
        case shader_file::ElementType::Float: return DXGI_FORMAT_R32_FLOAT;
        case shader_file::ElementType::Uint: return DXGI_FORMAT_R32_UINT;
        default: return DXGI_FORMAT_UNKNOWN;
    }
}

auto ionengine::renderer::rhi::element_type_size(rhi::shader_file::ElementType const element_type) -> uint32_t {

    switch(element_type) {
        case shader_file::ElementType::Float4: return sizeof(float) * 4;
        case shader_file::ElementType::Float4x4: return sizeof(float) * 16;
        case shader_file::ElementType::Float3: return sizeof(float) * 3;
        case shader_file::ElementType::Float3x3: return sizeof(float) * 9;
        case shader_file::ElementType::Float2: return sizeof(float) * 2;
        case shader_file::ElementType::Float2x2: return sizeof(float) * 4;
        case shader_file::ElementType::Float: return sizeof(float);
        case shader_file::ElementType::Uint: return sizeof(uint32_t);
        default: return 0;
    }
}

DX12Shader::DX12Shader(ID3D12Device4* device, std::span<uint8_t const> const data_bytes) {

    shader_file::ShaderFile shader_file(data_bytes);

    shader_name = shader_file.get_name();

    XXHash64 hasher(0);
    for(auto const& [stage, data] : shader_file.get_stages()) {
        std::vector<uint8_t> buffer;
        {
            auto shader_bytes = shader_file.get_buffer(data.buffer);
            buffer.resize(shader_bytes.size());
            std::memcpy(buffer.data(), shader_bytes.data(), buffer.size());
        }
        hasher.add(buffer.data(), buffer.size());
        buffers.emplace_back(buffer);

        auto d3d12_shader_bytecode = D3D12_SHADER_BYTECODE {};
        d3d12_shader_bytecode.pShaderBytecode = buffers.back().data();
        d3d12_shader_bytecode.BytecodeLength = buffers.back().size();

        stages.emplace(stage, d3d12_shader_bytecode);
    }
    hash = hasher.hash();

    {
        auto result = shader_file.get_stages().find(shader_file::ShaderStageType::Vertex);
        if(result != shader_file.get_stages().end()) {
            uint32_t offset = 0;
            for(auto const& input : result->second.inputs) {
                semantic_names.emplace_back(input.semantic);

                uint32_t index = 0;
                if(std::isdigit(input.semantic.back()) != 0) {
                    index = (int32_t)input.semantic.back() - 48;
                }

                auto d3d12_input_element = D3D12_INPUT_ELEMENT_DESC {};
                d3d12_input_element.SemanticName = semantic_names.back().data();
                d3d12_input_element.SemanticIndex = index;
                d3d12_input_element.Format = element_type_to_dxgi(input.element_type);
                d3d12_input_element.InputSlot = 0;
                d3d12_input_element.AlignedByteOffset = offset;
                d3d12_input_element.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
                d3d12_input_element.InstanceDataStepRate = 0;
                inputs.emplace_back(d3d12_input_element);

                offset += element_type_size(input.element_type);
            }
            inputs_size_per_vertex = result->second.inputs_size_per_vertex;
        }
    }

    for(auto const& [name, data] : shader_file.get_exports()) {
        bindings.emplace(name, data.binding);
    }
}