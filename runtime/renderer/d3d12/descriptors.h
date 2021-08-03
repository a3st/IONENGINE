// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class D3DDescriptorSetLayout : public DescriptorSetLayout {
public:

    D3DDescriptorSetLayout(const ComPtr<ID3D12Device4>& device, const std::vector<DescriptorSetLayoutBinding>& bindings) : m_device(device) {

		std::map<ShaderType, std::vector<D3D12_DESCRIPTOR_RANGE>> ranges_by_shader;

		for(auto& binding : bindings) {
			
			D3D12_DESCRIPTOR_RANGE range{};
			switch(binding.view_type) {
				case ViewType::Texture:
				case ViewType::StructuredBuffer:
				case ViewType::Buffer: range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; break;
				case ViewType::RWTexture:
				case ViewType::RWBuffer:
				case ViewType::RWStructuredBuffer: range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV; break;
				case ViewType::ConstantBuffer: range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV; break;
				case ViewType::Sampler: range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER; break;
			}
			range.NumDescriptors = binding.count;
			range.BaseShaderRegister = binding.slot;
			range.RegisterSpace = binding.space;
			range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			ranges_by_shader[binding.shader_type].emplace_back(range);
		}

		std::vector<D3D12_ROOT_PARAMETER> parameters;

		for(auto& range : ranges_by_shader) {
			
			D3D12_ROOT_PARAMETER parameter{};
			parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			parameter.DescriptorTable.NumDescriptorRanges = static_cast<uint32>(range.second.size());
			parameter.DescriptorTable.pDescriptorRanges = range.second.data();
			switch(range.first) {
				case ShaderType::Vertex: parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; break;
				case ShaderType::Pixel: parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; break;
				case ShaderType::Geometry: parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_GEOMETRY; break;
				case ShaderType::Hull: parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_HULL; break;
				case ShaderType::Domain: parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_DOMAIN; break;
				case ShaderType::All: parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; break;
				case ShaderType::Compute: parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; break;
			}

			parameters.emplace_back(parameter);
		}

        D3D12_ROOT_SIGNATURE_DESC root_desc{};
	    root_desc.NumParameters = static_cast<uint32>(parameters.size());
	    root_desc.pParameters = parameters.data();
	    root_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	    ComPtr<ID3DBlob> blob;
	    ASSERT_SUCCEEDED(D3D12SerializeRootSignature(&root_desc, D3D_ROOT_SIGNATURE_VERSION_1_0, blob.GetAddressOf(), nullptr));
	    ASSERT_SUCCEEDED(m_device->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&m_d3d12_root_signature)));
    }

	const ComPtr<ID3D12RootSignature>& get_root_signature() const { return m_d3d12_root_signature; }

private:

    const ComPtr<ID3D12Device4> m_device;

    ComPtr<ID3D12RootSignature> m_d3d12_root_signature;
};

}