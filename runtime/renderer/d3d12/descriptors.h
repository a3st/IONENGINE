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

class D3DDescriptorPool : public DescriptorPool {
public:

	D3DDescriptorPool(winrt::com_ptr<ID3D12Device4>& device, const std::vector<DescriptorPoolSize>& sizes) : m_device(device) {
		
		std::map<D3D12_DESCRIPTOR_HEAP_TYPE, uint32> descriptor_counts;
		for(auto& size : sizes) {

			descriptor_counts[convert_descriptor_heap_type(size.type)] += size.count;
		}

		m_d3d12_descriptor_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV] = nullptr;
		m_d3d12_descriptor_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER] = nullptr;
		m_d3d12_descriptor_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_RTV] = nullptr;
		m_d3d12_descriptor_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_DSV] = nullptr;

		for(auto& descriptor : descriptor_counts) {

			D3D12_DESCRIPTOR_HEAP_DESC heap_desc{};
			heap_desc.Type = descriptor.first;
			heap_desc.NumDescriptors = descriptor.second;
			heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			if(heap_desc.Type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV || heap_desc.Type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV) {
				heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			}

			m_device.get()->CreateDescriptorHeap(&heap_desc, __uuidof(ID3D12DescriptorHeap), m_d3d12_descriptor_heaps[descriptor.first].put_void());
		}
	}

private:

	std::reference_wrapper<winrt::com_ptr<ID3D12Device4>> m_device;

	std::map<D3D12_DESCRIPTOR_HEAP_TYPE, winrt::com_ptr<ID3D12DescriptorHeap>> m_d3d12_descriptor_heaps;

};

class D3DDescriptorSet : public DescriptorSet {
public:

	D3DDescriptorSet(winrt::com_ptr<ID3D12Device4>& device, DescriptorSetLayoutBinding& layout) : m_device(device), m_layout(layout) {
		
	}

	winrt::com_ptr<ID3D12DescriptorHeap>& get_descriptor_heap() { return m_d3d12_descriptor_heap; }

private:

	std::reference_wrapper<winrt::com_ptr<ID3D12Device4>> m_device;
	std::reference_wrapper<DescriptorSetLayoutBinding> m_layout;

	winrt::com_ptr<ID3D12DescriptorHeap> m_d3d12_descriptor_heap;

};

}