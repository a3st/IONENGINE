// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "renderer/rhi/shader.hpp"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <winrt/base.h>

namespace ionengine {

namespace renderer {

namespace rhi {

auto element_type_to_dxgi(rhi::shader_file::ElementType const element_type) -> DXGI_FORMAT;

auto element_type_size(rhi::shader_file::ElementType const element_type) -> uint32_t;

class DX12Shader : public Shader {
public:

    DX12Shader(ID3D12Device4* device, std::span<uint8_t const> const data_bytes);

    auto get_name() -> std::string_view {

        return shader_name;
    }

    auto get_inputs() -> std::span<D3D12_INPUT_ELEMENT_DESC const> {

        return inputs;
    }

    auto get_stages() -> std::unordered_map<rhi::shader_file::ShaderStageType, D3D12_SHADER_BYTECODE> const& {

        return stages;
    }

    auto get_bindings() -> std::unordered_map<std::string, uint32_t> const& {

        return bindings;
    }

private:

    std::string shader_name;
    std::unordered_map<rhi::shader_file::ShaderStageType, D3D12_SHADER_BYTECODE> stages;
    std::vector<D3D12_INPUT_ELEMENT_DESC> inputs;
    std::vector<std::string> semantic_names;
    std::vector<std::vector<uint8_t>> buffers;
    std::unordered_map<std::string, uint32_t> bindings;
};

}

}

}