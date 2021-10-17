// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

class D3DBindingSetLayout : public BindingSetLayout  {
public:

	struct Key {
		D3D12_DESCRIPTOR_HEAP_TYPE heap_type;
		D3D12_DESCRIPTOR_RANGE range;
		D3D12_SHADER_VISIBILITY shader_visibility;
	};

    D3DBindingSetLayout(ID3D12Device4* d3d12_device, const std::vector<BindingSetInputDesc>& bindings) {

		assert(d3d12_device && "pointer to d3d12_device is null");

		std::vector<Key> ranges;

		for(auto& binding : bindings) {
			
			D3D12_DESCRIPTOR_RANGE range{};
			range.RangeType = gfx_to_d3d12_descriptor_range_type(binding.view_type);
			range.NumDescriptors = binding.count;
			range.BaseShaderRegister = binding.slot;
			range.RegisterSpace = binding.space;
			range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			ranges.emplace_back(Key { gfx_to_d3d12_descriptor_heap_type(binding.view_type), range, gfx_to_d3d12_shader_visibility(binding.shader_type) });
		}

		std::map<D3D12_DESCRIPTOR_HEAP_TYPE, uint32> offsets;
		offsets[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV] = 0;
		offsets[D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER] = 0;

        std::vector<D3D12_ROOT_PARAMETER> parameters;

		for(auto& range : ranges) {
			
			D3D12_ROOT_PARAMETER parameter{};
			parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			parameter.DescriptorTable.NumDescriptorRanges = 1;
			parameter.DescriptorTable.pDescriptorRanges = &range.range;
			parameter.ShaderVisibility = range.shader_visibility;

			parameters.emplace_back(parameter);

			D3DDescriptorTable descriptor_table{};
			descriptor_table.heap_type = range.heap_type;
			descriptor_table.count = static_cast<uint32>(range.range.NumDescriptors);

			m_descriptor_tables.emplace_back(offsets[range.heap_type], descriptor_table);

			offsets[range.heap_type] += descriptor_table.count;
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