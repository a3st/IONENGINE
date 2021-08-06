// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class D3DDescriptorSetLayout : public DescriptorSetLayout {
public:

    D3DDescriptorSetLayout(winrt::com_ptr<ID3D12Device4>& device, const std::vector<DescriptorSetLayoutBinding>& bindings) : m_device(device) {

		std::map<ShaderType, std::vector<D3D12_DESCRIPTOR_RANGE>> ranges_by_shader;

		for(auto& binding : bindings) {
			
			D3D12_DESCRIPTOR_RANGE range{};
			range.RangeType = convert_enum(binding.view_type);
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
			parameter.ShaderVisibility = convert_enum(range.first);

			parameters.emplace_back(parameter);
		}

        D3D12_ROOT_SIGNATURE_DESC root_desc{};
	    root_desc.NumParameters = static_cast<uint32>(parameters.size());
	    root_desc.pParameters = parameters.data();
	    root_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	    winrt::com_ptr<ID3DBlob> blob;
	    ASSERT_SUCCEEDED(D3D12SerializeRootSignature(&root_desc, D3D_ROOT_SIGNATURE_VERSION_1_0, blob.put(), nullptr));
	    ASSERT_SUCCEEDED(m_device.get()->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), __uuidof(ID3D12RootSignature), m_d3d12_root_signature.put_void()));
    }

	winrt::com_ptr<ID3D12RootSignature>& get_root_signature() { return m_d3d12_root_signature; }

private:

    std::reference_wrapper<winrt::com_ptr<ID3D12Device4>> m_device;

    winrt::com_ptr<ID3D12RootSignature> m_d3d12_root_signature;
};

}