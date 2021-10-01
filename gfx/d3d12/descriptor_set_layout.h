// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

class D3DDescriptorSetLayout : public DescriptorSetLayout  {
public:

    D3DDescriptorSetLayout(ID3D12Device4* d3d12_device, const std::vector<DescriptorSetLayoutBinding>& bindings) {

        using Key = std::pair<D3D12_DESCRIPTOR_HEAP_TYPE, ShaderType>;
		std::map<Key, std::vector<D3D12_DESCRIPTOR_RANGE>> ranges;

		for(auto& binding : bindings) {
			
			D3D12_DESCRIPTOR_RANGE range{};
			range.RangeType = d3d12_descriptor_range_type_to_gfx_enum(binding.view_type);
			range.NumDescriptors = binding.count;
			range.BaseShaderRegister = binding.slot;
			range.RegisterSpace = binding.space;
			range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			ranges[{ d3d12_descriptor_heap_type_to_gfx_enum(binding.view_type), binding.shader_type }].emplace_back(range);
		}

		std::map<D3D12_DESCRIPTOR_HEAP_TYPE, uint32> offsets;
		offsets[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV] = 0;
		offsets[D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER] = 0;

        std::vector<D3D12_ROOT_PARAMETER> parameters;

		for(auto& range : ranges) {
			
			D3D12_ROOT_PARAMETER parameter{};
			parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			parameter.DescriptorTable.NumDescriptorRanges = static_cast<uint32>(range.second.size());
			parameter.DescriptorTable.pDescriptorRanges = range.second.data();
			parameter.ShaderVisibility = d3d12_shader_visibility_to_gfx_enum(range.first.second);

			parameters.emplace_back(parameter);

			D3DDescriptorTable descriptor_table{};
			descriptor_table.heap_type = range.first.first;
			descriptor_table.count = static_cast<uint32>(range.second.size());

			m_descriptor_tables.emplace_back(offsets[range.first.first], descriptor_table);

			offsets[range.first.first] += descriptor_table.count;
		}

        D3D12_ROOT_SIGNATURE_DESC root_desc{};
	    root_desc.NumParameters = static_cast<uint32>(parameters.size());
	    root_desc.pParameters = parameters.data();
	    root_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	    winrt::com_ptr<ID3DBlob> blob, error_blob;
	    THROW_IF_FAILED(D3D12SerializeRootSignature(&root_desc, D3D_ROOT_SIGNATURE_VERSION_1_0, blob.put(), error_blob.put()));
	    THROW_IF_FAILED(d3d12_device->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), __uuidof(ID3D12RootSignature), m_d3d12_root_signature.put_void()));
    }

    ID3D12RootSignature* get_d3d12_root_signature() { return m_d3d12_root_signature.get(); }

    const std::vector<std::pair<uint32, D3DDescriptorTable>>& get_descriptor_tables() const { return m_descriptor_tables; }

private:

    winrt::com_ptr<ID3D12RootSignature> m_d3d12_root_signature;

    std::vector<std::pair<uint32, D3DDescriptorTable>> m_descriptor_tables;
};

}